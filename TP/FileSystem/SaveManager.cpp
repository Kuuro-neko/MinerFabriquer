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

void SaveManager::loadPlayerData(Character &character)
{
    unsigned char gamemode;
    unsigned char prev;
    float position[3];
    int id, quantity;
    // no actual data -> create the folder
    if (isSaveFolderEmpty())
    {
        std::cout << "Player data file does not exist..." << std::endl;
        std::cout << "Creating a new player data file..." << std::endl;
        std::string saveFolder = generateSaveFolderPath();

        if (!std::filesystem::exists(saveFolder))
        {
            std::filesystem::create_directories(saveFolder);
        }

        std::ofstream file(saveFolder + PATH_PLAYER_FILE);
        std::ofstream ofs(saveFolder + PATH_PLAYER_FILE, std::ios::binary);
        std::cout << "Creation of the brand new Save folder at  :" << saveFolder + PATH_PLAYER_FILE << std::endl;

        // Default values from main.cpp
        gamemode = character.getGamemode();
        prev = character.GetprevGamemode();
        float position[3] = {
            character.getWorldPosition().x,
            character.getWorldPosition().y,
            character.getWorldPosition().z};

        // Write the data to the file
        ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
        ofs.write(reinterpret_cast<const char *>(&prev), sizeof(prev));
        ofs.write(reinterpret_cast<const char *>(position), sizeof(position));
        ofs.close();
        std::cout << "Player data file created and filled with default values." << std::endl;
    }
    else // the file is already created -> we load the data from the file
    {
        std::string motRecentFolder = getMostRecentSaveFolder();
        std::string mostRecentPlayerFilePath = motRecentFolder + PATH_PLAYER_FILE;

        std::cout
            << "Player data file already exists." << std::endl;
        std::cout << "Reading data from : " << motRecentFolder << std::endl;
        std::ifstream ifs(mostRecentPlayerFilePath, std::ios::binary);
        if (!ifs)
        {
            std::cerr << "Error opening file: " << mostRecentPlayerFilePath << std::endl;
            return;
        }

        // Read the data from the file
        ifs.read(reinterpret_cast<char *>(&gamemode), sizeof(gamemode));
        ifs.read(reinterpret_cast<char *>(&prev), sizeof(prev));
        ifs.read(reinterpret_cast<char *>(position), sizeof(position));
        while (ifs.read(reinterpret_cast<char *>(&id), sizeof(id)))
        {
            ifs.read(reinterpret_cast<char *>(&quantity), sizeof(quantity));
            // Add the item to the inventory
            character.inventory->addItem(ItemStack(id, quantity));
        }

        // Set the character's data
        character.setGamemode(gamemode);
        character.SetprevGamemode(prev);
        character.setWorldPosition(position[0], position[1], position[2]);
        ifs.close();
        std::cout << "Player data loaded from file." << std::endl;
    }
}

// create the correct folder based on time with the playerData.bin file
void SaveManager::saveCharacterFile(Character &data)
{
    std::string saveFolder = generateSaveFolderPath();

    if (!std::filesystem::exists(saveFolder))
    {
        std::filesystem::create_directories(saveFolder);
    }

    // File path
    std::string filePath = saveFolder + PATH_PLAYER_FILE;
    std::cout << "Creation of a new most Recent save folder : " << saveFolder + PATH_PLAYER_FILE << std::endl;

    std::ofstream ofs(filePath, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Erreur when trying to open the file : " << filePath << std::endl;
        return;
    }

    unsigned char gamemode = data.getGamemode();
    unsigned char prev = data.GetprevGamemode();
    float position[3] = {
        data.getWorldPosition().x,
        data.getWorldPosition().y,
        data.getWorldPosition().z};
    int id, quantity;

    ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
    ofs.write(reinterpret_cast<const char *>(&prev), sizeof(prev));
    ofs.write(reinterpret_cast<const char *>(position), sizeof(position));
    for (const auto &item : data.inventory->getItems())
    {
        id = item.getItemId();
        quantity = item.getQuantity();
        ofs.write(reinterpret_cast<const char *>(&id), sizeof(id));
        ofs.write(reinterpret_cast<const char *>(&quantity), sizeof(quantity));
    }
    ofs.close();

    std::cout << "Data saved at : " << filePath << std::endl;
}

void SaveManager::startAutoSave(Character &data)
{
    if (autoSaveRunning)
    {
        std::cerr << "Auto-save is already running." << std::endl;
        return;
    }

    autoSaveRunning = true;
    const std::string autoPlayerFilePath = generateSaveFolderPath() + PATH_PLAYER_FILE;
    int saveDelayFortheThread = SAVE_DELAY;

    // for the tread we have to define the lambda function and pass the data by reference
    autoSaveThread = std::thread([this, autoPlayerFilePath, &data, saveDelayFortheThread]()
                                 {
                                     while (autoSaveRunning)
                                     {
                                         std::this_thread::sleep_for(std::chrono::seconds(SAVE_DELAY));
                                         if (autoSaveRunning) // Check again to avoid race conditions
                                         {
                                             std::cout << "Auto-saving player data..." << std::endl;
                                             //save all the file that we need (player data, world data)
                                             saveCharacterFile(data);
                                             saveWorldFile();

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

bool SaveManager::isSaveFolderEmpty()
{
    const std::string saveDirectory = PATHSAVES;
    namespace fs = std::filesystem;

    fs::path dirPath(saveDirectory);

    if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
    {
        return false;
    }

    for (const auto &entry : fs::directory_iterator(dirPath))
    {
        if (entry.is_regular_file() || entry.is_directory())
        {
            return false; // On a trouvé au moins un fichier ou sous-dossier
        }
    }

    return true; // Aucun fichier utile
}

std::string SaveManager::getDate(long timestamp)
{
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm *tm = std::localtime(&time);

    char buffer[17]; // YYYY-MM-DD-HH-MM is 16 characters + null terminator
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M", tm);

    return std::string(buffer);
}

long SaveManager::getTimestamp()
{
    return static_cast<long>(std::time(nullptr));
}

std::string SaveManager::generateSaveFolderPath()
{
    std::string folderName = "save-" + getDate(getTimestamp());
    std::filesystem::path fullPath = std::filesystem::path(PATHSAVES) / folderName;
    return fullPath.string(); // convert to std::string
}

std::string SaveManager::getMostRecentSaveFolder()
{
    const std::string saveDirectory = PATHSAVES;
    namespace fs = std::filesystem;

    fs::path dirPath(saveDirectory);
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
    {
        return ""; // Return an empty string if the directory doesn't exist
    }

    std::string mostRecentFolder;
    std::time_t mostRecentTime = 0;

    for (const auto &entry : fs::directory_iterator(dirPath))
    {
        if (entry.is_directory())
        {
            std::string folderName = entry.path().filename().string();
            if (folderName.rfind("save-", 0) == 0) // Check if the folder name starts with "save-"
            {
                std::string timestampStr = folderName.substr(5); // Extract the timestamp part
                std::tm tm = {};
                std::istringstream ss(timestampStr);
                ss >> std::get_time(&tm, "%Y-%m-%d-%H-%M");
                if (!ss.fail())
                {
                    std::time_t folderTime = std::mktime(&tm);
                    if (folderTime > mostRecentTime)
                    {
                        mostRecentTime = folderTime;
                        mostRecentFolder = entry.path().string();
                    }
                }
            }
        }
    }

    return mostRecentFolder;
}

void SaveManager::setWorld(World *worldInstance)
{
    this->world = worldInstance;
}

void SaveManager::saveWorldFile()
{
    if (!world)
    {
        std::cerr << "World instance is not set. Cannot save world data." << std::endl;
        return;
    }
    std::cout << "Begin saving world data..." << std::endl;

    std::string saveFolder = generateSaveFolderPath();
    std::string filePath = saveFolder + PATH_WORLD_FILE;

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
            ChunkColumn *column = world->getChunkColumn(x, z);
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
            std::vector<VoxelChunk *> allChunks = column->getChunks();

            for (int i = 0; i < 8; ++i)
            {
                VoxelChunk *chunk = column->getChunk(i);
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
}

void SaveManager::loadWorldFile()
{

    std::string motRecentFolder = getMostRecentSaveFolder();
    std::string mostRecentWorldFilePath = motRecentFolder + PATH_WORLD_FILE;

    std::cout << "Reading data from : " << motRecentFolder << std::endl;
    std::ifstream ifs(mostRecentWorldFilePath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error opening file: " << mostRecentWorldFilePath << std::endl;
        return;
    }

    readWorldFile(ifs);
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
            ChunkColumn *column = new ChunkColumn(worldX, worldZ);
            world->addChunkColumn(column);

            // Read the heightmap data from the corresponding chunk column
            in.read(reinterpret_cast<char *>(column->getSurfaceHeightMap()), sizeof(column->getSurfaceHeightMap()));

            // Write the 8 chunks of the current column
            for (int i = 0; i < GENERATION_SIZE_Y; ++i)
            {
                VoxelChunk *chunk = world->createEmptyChunk(worldX, i, worldZ);
                for (int bx = 0; bx < CHUNK_SIZE; ++bx)
                {
                    for (int by = 0; by < CHUNK_SIZE; ++by)
                    {
                        for (int bz = 0; bz < CHUNK_SIZE; ++bz)
                        {
                            // Read block ID
                            uint16_t blockID;
                            in.read(reinterpret_cast<char *>(&blockID), sizeof(uint16_t));
                            chunk->setBloc(bx, by, bz, blockID);

                            // Read light level
                            uint8_t light;
                            in.read(reinterpret_cast<char *>(&light), sizeof(uint8_t));
                            chunk->setLightLevel(bx, by, bz, light);
                        }
                    }
                }
            }
        }
    }
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
