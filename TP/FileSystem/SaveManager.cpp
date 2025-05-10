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

    // Header Part that will contains the offset and lenght but for now ir's empty
    constexpr int REGION_WIDTH = GENERATION_SIZE_X;
    constexpr int REGION_DEPTH = GENERATION_SIZE_Z;
    constexpr int REGION_SIZE = REGION_WIDTH * REGION_DEPTH;

    // 1. Write and reserve the space for the header file
    std::vector<ChunkColumnEntry> toc(REGION_SIZE);                   // Table of Contents
    out.seekp(REGION_SIZE * sizeof(ChunkColumnEntry), std::ios::beg); // we save the space for futur adresse and content

    // 2. then we get all the data from the chunkColumn (posX, posY, heightmap, and the chunks)

    for (int z = 0; z < REGION_DEPTH; ++z)
    {
        for (int x = 0; x < REGION_WIDTH; ++x)
        {
            int index = z * REGION_WIDTH + x;
            std::shared_ptr<ChunkColumn> column = world->getChunkColumn(x, z);
            if (!column)
                continue;

            // 2.1 write the offset before writing new data
            std::streampos offset = out.tellp();

            // 2.2 save the data
            int32_t worldX = column->getChunkCoords().x;
            int32_t worldZ = column->getChunkCoords().y;
            out.write(reinterpret_cast<char *>(&worldX), sizeof(int32_t));
            out.write(reinterpret_cast<char *>(&worldZ), sizeof(int32_t));

            // 2.3 Sauvegarde heightmap
            out.write(reinterpret_cast<char *>(column->getSurfaceHeightMap()), sizeof(column->getSurfaceHeightMap()));

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
                            uint16_t blockID = chunk->getBloc(bx, by, bz);
                            out.write(reinterpret_cast<char *>(&blockID), sizeof(uint16_t));

                            // lightmap
                            uint8_t light = chunk->getLightLevelIncludingNeighbors(bx, by, bz);
                            out.write(reinterpret_cast<char *>(&light), sizeof(uint8_t));
                        }
                    }
                }
            }

            std::streampos end = out.tellp();
            toc[index].offset = static_cast<uint32_t>(offset);
            toc[index].length = static_cast<uint32_t>(end - offset);
        }
    }

    // 3. Revenir au début et écrire la vraie table des matières
    out.seekp(0, std::ios::beg);
    out.write(reinterpret_cast<char *>(toc.data()), REGION_SIZE * sizeof(ChunkColumnEntry));

    std::cout << "World saved to: " << filePath << std::endl;
    std::cout<< "Number of chunks saved : " << world->getAllChunks().size() << std::endl;
}
// TODO fix why not all chunks is loadings
void SaveManager::loadWorldFile()
{

    std::string motRecentFolder = getSaveFolderPath();
    std::string mostRecentWorldFilePath = motRecentFolder + PATH_WORLD_FILE;

  //
  //  std::cout << "Reading data from : " << motRecentFolder << std::endl;
    std::ifstream ifs(mostRecentWorldFilePath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error opening file: " << mostRecentWorldFilePath << std::endl;
        return;
    }

    readWorldFile(ifs);
    std::cout<< "Number of chunks read : " << world->getAllChunks().size() << std::endl;
}

void SaveManager::readWorldFile(std::ifstream &in)
{
    if (!world)
    {
        std::cerr << "World instance is not set. Cannot load world data." << std::endl;
        return;
    }

    constexpr int REGION_WIDTH = GENERATION_SIZE_X;
    constexpr int REGION_DEPTH = GENERATION_SIZE_Z;
    constexpr int REGION_SIZE = REGION_WIDTH * REGION_DEPTH; // 16 * 16 = 256

    //Read the header (table of contents)
    std::vector<ChunkColumnEntry> toc(REGION_SIZE);
    in.read(reinterpret_cast<char *>(toc.data()), REGION_SIZE * sizeof(ChunkColumnEntry));

    //Read the chunk column data
    for (int z = 0; z < REGION_DEPTH; ++z)
    {
        for (int x = 0; x < REGION_WIDTH; ++x)
        {
            int index = z * REGION_WIDTH + x;
            ChunkColumnEntry &entry = toc[index];

            // Skip empty entries
            if (entry.length == 0)
                continue;

            // Seek to the offset of the chunk column data
            in.seekg(entry.offset, std::ios::beg);

            // Read the chunk column metadata
            int32_t worldX, worldZ;
            in.read(reinterpret_cast<char *>(&worldX), sizeof(int32_t));
            in.read(reinterpret_cast<char *>(&worldZ), sizeof(int32_t));

            // Create a new chunk column
            std::shared_ptr<ChunkColumn> column = std::make_shared<ChunkColumn>(worldX, worldZ);
            column->initializeChunks();

            // Read the heightmap data from the corresponding chunk column
            in.read(reinterpret_cast<char *>(column->getSurfaceHeightMap()), sizeof(column->getSurfaceHeightMap()));

            // Write the 8 chunks of the current column
            for (int i = 0; i < GENERATION_SIZE_Y; ++i)
            {
                std::shared_ptr<VoxelChunk> chunk = column->getChunk(i);
                for (int bx = 0; bx < CHUNK_SIZE; ++bx)
                {
                    for (int by = 0; by < CHUNK_SIZE; ++by)
                    {
                        for (int bz = 0; bz < CHUNK_SIZE; ++bz)
                        {
                            // Read block ID
                            uint16_t blockID;
                            in.read(reinterpret_cast<char *>(&blockID), sizeof(uint16_t));
                            chunk->generationSetBloc(bx, by, bz, blockID);

                            // Read light level
                            uint8_t light;
                            in.read(reinterpret_cast<char *>(&light), sizeof(uint8_t));
                            chunk->setLightLevel(bx, by, bz, light);
                        }
                    }
                }
            }

            // Add the chunk column to the world
            world->addColumn(column);
            column->assignWorld(world);
            for (auto &chunk : column->getChunks()) {
                world->emplaceChunk(chunk);
            }
        }
    }
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
