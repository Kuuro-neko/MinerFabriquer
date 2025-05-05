#include "SaveManager.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <filesystem>
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

    ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
    ofs.write(reinterpret_cast<const char *>(&prev), sizeof(prev));
    ofs.write(reinterpret_cast<const char *>(position), sizeof(position));
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
    autoSaveThread = std::thread([this, autoPlayerFilePath, &data, saveDelayFortheThread]()
                                 {
                                     while (autoSaveRunning)
                                     {
                                         std::this_thread::sleep_for(std::chrono::seconds(SAVE_DELAY));
                                         if (autoSaveRunning) // Check again to avoid race conditions
                                         {
                                             std::cout << "Auto-saving player data..." << std::endl;
                                             saveCharacterFile(data);

                                             std::cout << "Player data auto-saved to file: " << autoPlayerFilePath << std::endl;
                                         }
                                     } });
}

void SaveManager::stopAutoSave()
{
    if (autoSaveRunning)
    {
        autoSaveRunning = false;
        if (autoSaveThread.joinable())
        {
            autoSaveThread.join();
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
