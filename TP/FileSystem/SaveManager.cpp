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

    std::string seedStr = WorldGenerator::getInstance().getSeedStr();
    int seedStrSize = seedStr.size();
    ofs.write(reinterpret_cast<const char *>(&seedStrSize), sizeof(int));
    if (seedStrSize > 0) {
        ofs.write(seedStr.data(), seedStrSize); // Write string content
    }
    std::cout << "Seed file created at: " << saveFolder + PATH_SEED << std::endl;
}

void SaveManager::saveSeedFile()
{
    int seedStrSize = 0;
    std::string seedStr;

    std::string saveFolder = getSaveFolderPath();
    std::cout << "Saving seed at : " << saveFolder + PATH_SEED << std::endl;
    std::ofstream ofs(saveFolder + PATH_SEED, std::ios::binary);

    if (!ofs)
    {
        std::cerr << "Error creating seed file." << std::endl;
        return;
    }

    seedStr = WorldGenerator::getInstance().getSeedStr();
    seedStrSize = seedStr.size();
    ofs.write(reinterpret_cast<const char *>(&seedStrSize), sizeof(int));
    if (seedStrSize > 0)
    {
        ofs.write(seedStr.data(), seedStrSize); // Write string content
    }
    std::cout << "Seed file created at: " << saveFolder + PATH_SEED << std::endl;
}

void SaveManager::readSeedFile()
{
    int seedStrSize = 0;
    std::string seedStr;

    std::string mostRecentSeedFilePath = getSaveFolderPath() + PATH_SEED;
    std::ifstream ifs(mostRecentSeedFilePath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error opening file: " << mostRecentSeedFilePath << std::endl;
        return;
    }
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

    unsigned char gamemode = character->getGamemode();
    unsigned char prev = character->GetprevGamemode();
    float position[3] = {
            character->getWorldPosition().x,
            character->getWorldPosition().y,
            character->getWorldPosition().z};

    ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
    ofs.write(reinterpret_cast<const char *>(&prev), sizeof(prev));
    ofs.write(reinterpret_cast<const char *>(position), sizeof(position));
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

    unsigned char gamemode;
    unsigned char prev;
    float position[3];
    int id, quantity;

    ifs.read(reinterpret_cast<char *>(&gamemode), sizeof(gamemode));
    ifs.read(reinterpret_cast<char *>(&prev), sizeof(prev));
    ifs.read(reinterpret_cast<char *>(position), sizeof(position));

    while (ifs.read(reinterpret_cast<char *>(&id), sizeof(id)))
    {
        ifs.read(reinterpret_cast<char *>(&quantity), sizeof(quantity));
        character->inventory->addItem(ItemStack(id, quantity));
    }

    character->setGamemode(gamemode);
    character->SetprevGamemode(prev);
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

    unsigned char gamemode = character->getGamemode();
    unsigned char prev = character->GetprevGamemode();
    float position[3] = {
            character->getWorldPosition().x,
            character->getWorldPosition().y,
            character->getWorldPosition().z};
    int id, quantity;

    ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
    ofs.write(reinterpret_cast<const char *>(&prev), sizeof(prev));
    ofs.write(reinterpret_cast<const char *>(position), sizeof(position));
    for (const auto &item : character->inventory->getItems())
    {
        id = item.getItemId();
        quantity = item.getQuantity();
        ofs.write(reinterpret_cast<const char *>(&id), sizeof(id));
        ofs.write(reinterpret_cast<const char *>(&quantity), sizeof(quantity));
    }
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
                                             saveSeedFile();
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

    // TODO pour le moment on ne fait que la region actuelle mais faudra ajouter un system pour szuvgarder de nouvelle region

    // Creation du header de la region actuelle

    std::ofstream out(filePath, std::ios::binary);
    if (!out)
    {
        std::cerr << "Impossible to create the Saving file" << std::endl;
    }


    int countCol = 0;
    int countChunk = 0;
    int colTotal = 0;
    {
        std::unique_lock<std::recursive_mutex> worldLock(world->worldMutex);
        std::vector<std::shared_ptr<ChunkColumn>> columns = world->getAllColumns();
        colTotal = columns.size();
    
        for (auto &column : columns)
        {
            // 2.2 save the data
            int32_t worldX = column->getChunkCoords().x;
            int32_t worldZ = column->getChunkCoords().y;
            out.write(reinterpret_cast<char *>(&worldX), sizeof(int32_t));
            out.write(reinterpret_cast<char *>(&worldZ), sizeof(int32_t));
        
            // 2.3 Sauvegarde heightmap
            out.write(reinterpret_cast<char *>(column->getSurfaceHeightMap()), sizeof(int32_t) * 16 * 16);
        
            // 2.4 Sauvegarde des 8 chunks
            std::vector<std::shared_ptr<VoxelChunk>> allChunks = column->getChunks();
        
            for (int i = 0; i < 8; ++i)
            {
                std::shared_ptr<VoxelChunk> chunk = column->getChunk(i);
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
            countCol++;
        }
    }


    std::cout << "World saved to: " << filePath << std::endl;
    std::cout<< "Number of columns saved : " << countCol << " / " << colTotal << std::endl;
    std::cout<< "Number of chunks saved : " << countChunk << " / " << colTotal * 8 << std::endl;

}
// TODO fix why not all chunks is loadings
std::vector<SaveManager::ChunkColumnEntry> SaveManager::loadWorldFile()
{

    std::string motRecentFolder = getSaveFolderPath();
    std::string mostRecentWorldFilePath = motRecentFolder + PATH_WORLD_FILE;

  //
  //  std::cout << "Reading data from : " << motRecentFolder << std::endl;
    std::ifstream ifs(mostRecentWorldFilePath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error opening file: " << mostRecentWorldFilePath << std::endl;
        exit(1);
    }

    return readWorldFile(ifs);
    std::cout<< "Number of chunks read : " << world->getAllChunks().size() << std::endl;
}

std::vector<SaveManager::ChunkColumnEntry> SaveManager::readWorldFile(std::ifstream &in)
{
    if (!world)
    {
        std::cerr << "World instance is not set. Cannot load world data." << std::endl;
        exit(1);
    }

    // Check if the stream is good and not at EOF
    if (!in.good()) {
        std::cerr << "Input stream is in bad state." << std::endl;
        return {};
    }

    // Reset file position to beginning if needed
    in.seekg(0, std::ios::beg);

    // Check file size
    in.seekg(0, std::ios::end);
    std::streampos fileSize = in.tellg();
    in.seekg(0, std::ios::beg);
    
    if (fileSize == 0) {
        std::cerr << "File is empty." << std::endl;
        return {};
    }
    
    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    std::vector<ChunkColumnEntry> chunkColumnEntries;
    int countCol = 0;
    int countChunk = 0;
    
    while(in && in.peek() != EOF) {
        ChunkColumnEntry columnEntry = {};
        
        // Read column coordinates
        if (!in.read(reinterpret_cast<char *>(&columnEntry.worldX), sizeof(int32_t)) ||
            !in.read(reinterpret_cast<char *>(&columnEntry.worldZ), sizeof(int32_t))) {
            std::cerr << "Failed to read column coordinates" << std::endl;
            break;
        }
        
        // Read heightmap
        if (!in.read(reinterpret_cast<char *>(columnEntry.heightmap), sizeof(int32_t) * 16 * 16)) {
            std::cerr << "Failed to read heightmap" << std::endl;
            break;
        }
        auto chunkReadError = false;
        for (int i = 0; i < 8; ++i)
        {
            ChunkEntry chunkEntry = {};
            for (int bx = 0; bx < CHUNK_SIZE; ++bx)
            {
                for (int by = 0; by < CHUNK_SIZE; ++by)
                {
                    for (int bz = 0; bz < CHUNK_SIZE; ++bz)
                    {
                        // Read block ID
                        int8_t blockID;
                        if (!in.read(reinterpret_cast<char *>(&blockID), sizeof(int8_t))) {
                            std::cerr << "Failed to read block ID at chunk " << i 
                                      << " position (" << bx << "," << by << "," << bz << ")" << std::endl;
                            chunkReadError = true;
                            break;
                        }
                        chunkEntry.blocksID.push_back(blockID);

                        // Read light level
                        int8_t light;
                        if (!in.read(reinterpret_cast<char *>(&light), sizeof(int8_t))) {
                            std::cerr << "Failed to read light level at chunk " << i 
                                      << " position (" << bx << "," << by << "," << bz << ")" << std::endl;
                            chunkReadError = true;
                            break;
                        }
                        chunkEntry.lightmap.push_back(light);
                    }
                }
            }
            
            if (!chunkReadError) {
                columnEntry.chunks[i] = chunkEntry;
                countChunk++;
            } else {
                break;
            }
        }
        
        if (!chunkReadError) {
            chunkColumnEntries.push_back(columnEntry);
            countCol++;
        } else {
            break;
        }
    }
    
    std::cout << "Number of columns read: " << countCol << std::endl;
    std::cout << "Number of chunks read: " << countChunk << std::endl;
    std::cout << "Size of chunkColumnEntries: " << chunkColumnEntries.size() << std::endl;
    
    // If we didn't read anything, provide more specific debug info
    if (countCol == 0) {
        std::cerr << "Warning: No data was read from the file." << std::endl;
        std::cerr << "File stream state: " << 
            (in.good() ? "good" : "") << 
            (in.eof() ? " eof" : "") << 
            (in.fail() ? " fail" : "") << 
            (in.bad() ? " bad" : "") << std::endl;
    }
    
    return chunkColumnEntries;
}

bool SaveManager::isDataFolderContainsOtherFolder() {
    for (const auto &entry: std::filesystem::directory_iterator(PATHSAVES)) {
        if (entry.is_directory()) {
            std::cout << "Found folder: " << entry.path().filename().string() << std::endl;
            return true;
        }
    }
    std::cout << "No folders found in PATHSAVES." << std::endl;
    return false;
}



// TODO : pour l'oral parler des types de représentation qui existait avec pour t contre
// JSON -> lisible mais pas optimisé quand il faut parcourir beaucoup de données + lourd
// NBT -> format standar créer par mojang pour le jeu Minecraft, il est optimisé pour la vitesse de lecture et d'écriture, mais pas lisible par l'homme mais hyper légéer
// fonctionne via un arbre binaire de tag qui permet de stocker et de récupérer les données rapidement sans avoir a tout parcourir via les tag

/* great way to test
SaveManager &saveManager1 = SaveManager::getInstance();
    SaveManager &saveManager2 = SaveManager::getInstance();

    if (&saveManager1 == &saveManager2)
    {
        std::cout << "Singleton fonctionne : les deux instances sont identiques." << std::endl;
    }
    else
    {
        std::cout << "Erreur : les instances sont différentes !" << std::endl;
    }
*/
