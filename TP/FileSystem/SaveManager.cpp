#include "SaveManager.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

// if the instance is null, create it else return the existing one -> thread safe
SaveManager &SaveManager::getInstance()
{
    static SaveManager instance; // local static variable
    return instance;
}

void SaveManager::createSeedFile()
{
    std::string saveFolder = getSaveFolderPath();

    std::cout << "Creation of a new save folder for character : " << saveFolder + PATH_SEED << std::endl;
    std::ofstream ofs(saveFolder + PATH_SEED, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Error creating seed file." << std::endl;
        return;
    }

    // Create and write header
    SeedHeader header;
    header.seedDataOffset = sizeof(SeedHeader);

    // Reserve space for header (we'll update it later)
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(SeedHeader));

    // Write seed data
    std::string seedStr = WorldGenerator::getInstance().getSeedStr();
    int seedStrSize = seedStr.size();

    // Record position where seed data starts
    header.seedDataOffset = ofs.tellp();

    // Write seed data
    ofs.write(reinterpret_cast<const char *>(&seedStrSize), sizeof(int));
    if (seedStrSize > 0)
    {
        ofs.write(seedStr.data(), seedStrSize); // Write string content
    }

    // Update header with final sizes
    header.seedDataSize = static_cast<uint32_t>(sizeof(int) + seedStrSize);
    header.totalFileSize = static_cast<uint32_t>(ofs.tellp());

    // Write final header
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(SeedHeader));

    std::cout << "Seed file created at: " << saveFolder + PATH_SEED << std::endl;
}

void SaveManager::saveSeedFile()
{
    std::string saveFolder = getSaveFolderPath();
    std::cout << "Saving seed at : " << saveFolder + PATH_SEED << std::endl;
    std::ofstream ofs(saveFolder + PATH_SEED, std::ios::binary);

    if (!ofs)
    {
        std::cerr << "Error creating seed file." << std::endl;
        return;
    }

    // Create and write header
    SeedHeader header;
    header.seedDataOffset = sizeof(SeedHeader);

    // Reserve space for header (we'll update it later)
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(SeedHeader));

    // Write seed data
    std::string seedStr = WorldGenerator::getInstance().getSeedStr();
    int seedStrSize = seedStr.size();

    // Record position where seed data starts
    header.seedDataOffset = ofs.tellp();

    // Write seed data
    ofs.write(reinterpret_cast<const char *>(&seedStrSize), sizeof(int));
    if (seedStrSize > 0)
    {
        ofs.write(seedStr.data(), seedStrSize);
    }

    // Update header with final sizes
    header.seedDataSize = static_cast<uint32_t>(sizeof(int) + seedStrSize);
    header.totalFileSize = static_cast<uint32_t>(ofs.tellp());

    // Write final header
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(SeedHeader));

    std::cout << "Seed file created at: " << saveFolder + PATH_SEED << std::endl;
}

void SaveManager::readSeedFile()
{
    std::string mostRecentSeedFilePath = getSaveFolderPath() + PATH_SEED;
    std::ifstream ifs(mostRecentSeedFilePath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error opening file: " << mostRecentSeedFilePath << std::endl;
        return;
    }

    // Read header
    SeedHeader header;
    ifs.read(reinterpret_cast<char *>(&header), sizeof(SeedHeader));

    // Seek to seed data
    ifs.seekg(header.seedDataOffset);

    // Read seed data
    int seedStrSize = 0;
    std::string seedStr;

    ifs.read(reinterpret_cast<char *>(&seedStrSize), sizeof(int));
    if (seedStrSize > 0)
    {
        seedStr.resize(seedStrSize);
        ifs.read(&seedStr[0], seedStrSize); // Read string content
        WorldGenerator::getInstance().setSeed(seedStr);
    }
    else
    {
        std::cerr << "Error reading seed file." << std::endl;
    }
    std::cout << "Seed file read at: " << mostRecentSeedFilePath << std::endl;
}

void SaveManager::createPlayerDataFile()
{
    std::string saveFolder = getSaveFolderPath();

    std::cout << "Creation of a new save folder for character : " << saveFolder + PATH_PLAYER_FILE << std::endl;
    std::ofstream ofs(saveFolder + PATH_PLAYER_FILE, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Error creating player data file." << std::endl;
        return;
    }

    // Create and write header
    PlayerDataHeader header;

    // Write placeholder for header (we'll update it later)
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(PlayerDataHeader));

    // Character data section
    header.characterDataOffset = static_cast<uint32_t>(ofs.tellp());

    unsigned char gamemode = character->getGamemode();
    float position[3] = {
        character->getWorldPosition().x,
        character->getWorldPosition().y,
        character->getWorldPosition().z};

    ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
    ofs.write(reinterpret_cast<const char *>(position), sizeof(position));

    header.characterDataSize = static_cast<uint32_t>(ofs.tellp() - header.characterDataOffset);
    header.inventoryOffset = static_cast<uint32_t>(ofs.tellp());

    // No inventory data in createPlayerDataFile currently
    header.inventorySize = 0;

    // Update total file size
    header.totalFileSize = static_cast<uint32_t>(ofs.tellp());

    // Write final header
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(PlayerDataHeader));

    ofs.close();
    std::cout << "Player data file created at: " << saveFolder + PATH_PLAYER_FILE << std::endl;
}

void SaveManager::loadPlayerData()
{
    std::string mostRecentPlayerFilePath = getSaveFolderPath() + PATH_PLAYER_FILE;

    std::ifstream ifs(mostRecentPlayerFilePath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error opening file: " << mostRecentPlayerFilePath << std::endl;
        return;
    }

    // Read header
    PlayerDataHeader header;
    ifs.read(reinterpret_cast<char *>(&header), sizeof(PlayerDataHeader));

    // Read character data
    ifs.seekg(header.characterDataOffset);

    unsigned char gamemode;
    float position[3];

    ifs.read(reinterpret_cast<char *>(&gamemode), sizeof(gamemode));
    ifs.read(reinterpret_cast<char *>(position), sizeof(position));

    // Read inventory if present
    if (header.inventorySize > 0)
    {
        ifs.seekg(header.inventoryOffset);

        int id, quantity;
        while (ifs.tellg() < header.inventoryOffset + header.inventorySize &&
               ifs.read(reinterpret_cast<char *>(&id), sizeof(id)))
        {
            ifs.read(reinterpret_cast<char *>(&quantity), sizeof(quantity));
            character->inventory->addItem(ItemStack(id, quantity));
        }
    }

    character->setGamemode(gamemode);
    character->setWorldPosition(position[0], position[1], position[2]);
    ifs.close();
}

// create the correct folder based on time with the playerData.bin file
void SaveManager::saveCharacterFile()
{
    std::string saveFolder = getSaveFolderPath();

    if (!std::filesystem::exists(saveFolder))
    {
        std::filesystem::create_directories(saveFolder);
    }

    // File path
    std::string filePath = saveFolder + PATH_PLAYER_FILE;
    std::cout << "Creation of a new save for character : " << filePath << std::endl;

    std::ofstream ofs(filePath, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Erreur when trying to open the file : " << filePath << std::endl;
        return;
    }

    // Create and write header placeholder
    PlayerDataHeader header;
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(PlayerDataHeader));

    // Character data section
    header.characterDataOffset = static_cast<uint32_t>(ofs.tellp());

    unsigned char gamemode = character->getGamemode();
    float position[3] = {
        character->getWorldPosition().x,
        character->getWorldPosition().y,
        character->getWorldPosition().z};

    ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
    ofs.write(reinterpret_cast<const char *>(position), sizeof(position));

    header.characterDataSize = static_cast<uint32_t>(ofs.tellp() - header.characterDataOffset);

    // Inventory section
    header.inventoryOffset = static_cast<uint32_t>(ofs.tellp());

    auto inventoryStartPos = ofs.tellp();
    for (const auto &item : character->inventory->getItems())
    {
        int id = item.getItemId();
        int quantity = item.getQuantity();
        ofs.write(reinterpret_cast<const char *>(&id), sizeof(id));
        ofs.write(reinterpret_cast<const char *>(&quantity), sizeof(quantity));
    }

    header.inventorySize = static_cast<uint32_t>(ofs.tellp() - inventoryStartPos);
    header.totalFileSize = static_cast<uint32_t>(ofs.tellp());

    // Write final header
    ofs.seekp(0);
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(PlayerDataHeader));

    ofs.close();
    std::cout << "Data saved at : " << filePath << std::endl;
}

void SaveManager::startAutoSave()
{
    if (autoSaveRunning)
    {
        std::cerr << "Auto-save is already running." << std::endl;
        return;
    }

    autoSaveRunning = true;
    const std::string autoPlayerFilePath = getSaveFolderPath() + PATH_PLAYER_FILE;
    int saveDelayFortheThread = SAVE_DELAY;
    // for the tread we have to define the lambda function and pass the data by reference
    autoSaveThread = std::thread([this, autoPlayerFilePath, saveDelayFortheThread]()
                                 {
                                     while (autoSaveRunning)
                                     {
                                         std::this_thread::sleep_for(std::chrono::seconds(SAVE_DELAY));
                                         if (autoSaveRunning) // Check again to avoid race conditions
                                         {
                                             std::cout << "Auto-saving player data..." << std::endl;
                                             //save all the file that we need (player data, world data)
                                             std::cout << "Auto-saving world data..." << std::endl;
                                             saveWorldFile();
                                             saveCharacterFile();
                                             std::cout
                                                     << "Player data auto-saved to file: " << autoPlayerFilePath << std::endl;
                                         }

                                     } });
}

void SaveManager::stopAutoSave()
{
    if (autoSaveRunning)
    {
        autoSaveRunning = false; // Signal the thread to stop
        autoSaveThread.detach();
        if (autoSaveThread.joinable())
        {
            std::cout << "Waiting for auto-save thread to finish..." << std::endl;
            autoSaveThread.join(); // Wait for the thread to finish
        }
        else
        {
            std::cerr << "Auto-save thread is not joinable." << std::endl;
        }

        std::cout << "Auto-save stopped." << std::endl;
    }
}

void SaveManager::setWorld(World *worldInstance)
{
    this->world = worldInstance;
}

void SaveManager::setCharacter(Character *characterInstance)
{
    this->character = characterInstance;
}

void SaveManager::saveWorldFile()
{
    if (!world)
    {
        std::cerr << "World instance is not set. Cannot save world data." << std::endl;
        return;
    }
    std::cout << "Begin saving world data..." << std::endl;

    std::string filePath = getSaveFolderPath() + PATH_WORLD_FILE;
    std::ofstream out(filePath, std::ios::binary);
    if (!out)
    {
        std::cerr << "Impossible to create the Saving file" << std::endl;
        return;
    }

    // Create and write header placeholder
    WorldFileHeader header;
    out.write(reinterpret_cast<char *>(&header), sizeof(WorldFileHeader));

    // Get all columns
    std::vector<std::shared_ptr<ChunkColumn>> columns;
    {
        std::unique_lock<std::recursive_mutex> worldLock(world->worldMutex);
        columns = world->getAllColumns();
    }

    header.columnCount = columns.size();

    // Create space for the column table (will fill it later)
    header.columnsTableOffset = sizeof(WorldFileHeader);
    std::vector<ColumnTableEntry> columnTable(columns.size());

    // Reserve space for column table
    out.seekp(header.columnsTableOffset);
    out.write(reinterpret_cast<char *>(columnTable.data()), sizeof(ColumnTableEntry) * columnTable.size());

    // Start writing actual data
    header.dataStartOffset = static_cast<uint32_t>(out.tellp());

    int countCol = 0;
    int countChunk = 0;

    for (size_t i = 0; i < columns.size(); i++)
    {
        auto &column = columns[i];

        // Record the start position for this column's data
        columnTable[i].worldX = column->getChunkCoords().x;
        columnTable[i].worldZ = column->getChunkCoords().y;
        columnTable[i].dataOffset = static_cast<uint32_t>(out.tellp());

        // Save heightmap
        out.write(reinterpret_cast<char *>(column->getSurfaceHeightMap()), sizeof(int32_t) * 16 * 16);

        // Save chunks
        std::vector<std::shared_ptr<VoxelChunk>> allChunks = column->getChunks();

        for (int j = 0; j < 8; ++j)
        {
            std::shared_ptr<VoxelChunk> chunk = column->getChunk(j);
            for (int bx = 0; bx < CHUNK_SIZE; ++bx)
            {
                for (int by = 0; by < CHUNK_SIZE; ++by)
                {
                    for (int bz = 0; bz < CHUNK_SIZE; ++bz)
                    {
                        // blocID
                        int8_t blockID = chunk->getBloc(bx, by, bz);
                        out.write(reinterpret_cast<char *>(&blockID), sizeof(int8_t));

                        // lightmap
                        int8_t light = chunk->getLightLevel(bx, by, bz);
                        out.write(reinterpret_cast<char *>(&light), sizeof(int8_t));
                    }
                }
            }
            countChunk++;
        }

        // Record the length of this column's data
        columnTable[i].dataLength = static_cast<uint32_t>(out.tellp() - columnTable[i].dataOffset);
        countCol++;
    }

    // Update total file size
    header.totalFileSize = static_cast<uint32_t>(out.tellp());

    // Write the updated header
    out.seekp(0);
    out.write(reinterpret_cast<char *>(&header), sizeof(WorldFileHeader));

    // Write the updated column table
    out.seekp(header.columnsTableOffset);
    out.write(reinterpret_cast<char *>(columnTable.data()), sizeof(ColumnTableEntry) * columnTable.size());

    std::cout << "World saved to: " << filePath << std::endl;
    std::cout << "Number of columns saved : " << countCol << " / " << columns.size() << std::endl;
    std::cout << "Number of chunks saved : " << countChunk << " / " << columns.size() * 8 << std::endl;
}

std::vector<SaveManager::ChunkColumnEntry> SaveManager::loadWorldFile()
{
    std::string mostRecentWorldFilePath = getSaveFolderPath() + PATH_WORLD_FILE;
    std::cout << "Reading world data from: " << mostRecentWorldFilePath << std::endl;

    std::ifstream in(mostRecentWorldFilePath, std::ios::binary);
    if (!in)
    {
        std::cerr << "Error opening file: " << mostRecentWorldFilePath << std::endl;
        exit(1);
    }

    return readWorldFile(in);
}

std::vector<SaveManager::ChunkColumnEntry> SaveManager::readWorldFile(std::ifstream &in)
{
    if (!world)
    {
        std::cerr << "World instance is not set. Cannot load world data." << std::endl;
        exit(1);
    }

    // Check if the stream is good
    if (!in.good())
    {
        std::cerr << "Input stream is in bad state." << std::endl;
        return {};
    }

    // Reset file position to beginning
    in.seekg(0, std::ios::beg);

    // Check file size
    in.seekg(0, std::ios::end);
    std::streampos fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    if (fileSize == 0)
    {
        std::cerr << "File is empty." << std::endl;
        return {};
    }

    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    // Read header
    WorldFileHeader header;
    in.read(reinterpret_cast<char *>(&header), sizeof(WorldFileHeader));

    std::cout << "Reading " << header.columnCount << " columns from file" << std::endl;

    // Read column table
    std::vector<ColumnTableEntry> columnTable(header.columnCount);
    in.seekg(header.columnsTableOffset);
    in.read(reinterpret_cast<char *>(columnTable.data()), sizeof(ColumnTableEntry) * columnTable.size());

    // Process each column according to the table
    std::vector<ChunkColumnEntry> chunkColumnEntries;
    int countCol = 0;
    int countChunk = 0;

    for (const auto &tableEntry : columnTable)
    {
        ChunkColumnEntry columnEntry = {};
        columnEntry.worldX = tableEntry.worldX;
        columnEntry.worldZ = tableEntry.worldZ;
        columnEntry.offset = tableEntry.dataOffset;
        columnEntry.length = tableEntry.dataLength;

        // Seek to this column's data
        in.seekg(tableEntry.dataOffset);

        // Read heightmap
        in.read(reinterpret_cast<char *>(columnEntry.heightmap), sizeof(int32_t) * 16 * 16);

        // Read chunks
        for (int i = 0; i < 8; ++i)
        {
            ChunkEntry chunkEntry = {};

            // Pre-allocate vectors for performance
            chunkEntry.blocksID.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
            chunkEntry.lightmap.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

            for (int bx = 0; bx < CHUNK_SIZE; ++bx)
            {
                for (int by = 0; by < CHUNK_SIZE; ++by)
                {
                    for (int bz = 0; bz < CHUNK_SIZE; ++bz)
                    {
                        // Read block ID
                        int8_t blockID;
                        if (!in.read(reinterpret_cast<char *>(&blockID), sizeof(int8_t)))
                        {
                            std::cerr << "Failed to read block ID at chunk " << i
                                      << " position (" << bx << "," << by << "," << bz << ")" << std::endl;
                            break;
                        }
                        chunkEntry.blocksID.push_back(blockID);

                        // Read light level
                        int8_t light;
                        if (!in.read(reinterpret_cast<char *>(&light), sizeof(int8_t)))
                        {
                            std::cerr << "Failed to read light level at chunk " << i
                                      << " position (" << bx << "," << by << "," << bz << ")" << std::endl;
                            break;
                        }
                        chunkEntry.lightmap.push_back(light);
                    }
                }
            }

            columnEntry.chunks[i] = chunkEntry;
            countChunk++;
        }

        chunkColumnEntries.push_back(columnEntry);
        countCol++;
    }

    std::cout << "Number of columns read: " << countCol << std::endl;
    std::cout << "Number of chunks read: " << countChunk << std::endl;

    return chunkColumnEntries;
}

bool SaveManager::isDataFolderContainsOtherFolder()
{
    for (const auto &entry : std::filesystem::directory_iterator(PATHSAVES))
    {
        if (entry.is_directory())
        {
            std::cout << "Found folder: " << entry.path().filename().string() << std::endl;
            return true;
        }
    }
    std::cout << "No folders found in PATHSAVES." << std::endl;
    return false;
}