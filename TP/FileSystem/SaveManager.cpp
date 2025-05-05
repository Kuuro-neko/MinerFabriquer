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
// return if the file has been created at least once
bool SaveManager::isPlayerDataFileExist(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

void SaveManager::loadPlayerData(Character &character, const std::string &filename)
{
    unsigned char gamemode;
    unsigned char prev;
    float position[3];
    // the file is not created yet -> we create it
    if (!isPlayerDataFileExist(filename))
    {
        std::cout << "Player data file does not exist." << std::endl;
        std::cout << "Creating a new player data file." << std::endl;
        std::ofstream file(filename);
        std::ofstream ofs(filename, std::ios::binary);

        // Default values (that's on default values)
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

        std::cout << "Player data file already exists." << std::endl;
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs)
        {
            std::cerr << "Error opening file: " << filename << std::endl;
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

void SaveManager::readPlayerDataFromFile(const std::string &filename, Character &data)
{
}

void SaveManager::saveCharacterFile(Character &data)
{

    std::ofstream ofs(PATHPLAYERFILE, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Error opening file for manual save: " << PATHPLAYERFILE << std::endl;
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

    std::cout << "Player data manually saved to file: " << PATHPLAYERFILE << std::endl;
}

void SaveManager::startAutoSave(Character &data)
{
    if (autoSaveRunning)
    {
        std::cerr << "Auto-save is already running." << std::endl;
        return;
    }

    autoSaveRunning = true;
    const std::string pathForThread = PATHPLAYERFILE;
    int saveDelayFortheThread = SAVE_DELAY;
    autoSaveThread = std::thread([this, pathForThread, &data, saveDelayFortheThread]()
                                 {
                                     while (autoSaveRunning)
                                     {
                                         std::this_thread::sleep_for(std::chrono::seconds(SAVE_DELAY));
                                         if (autoSaveRunning) // Check again to avoid race conditions
                                         {
                                             std::cout << "Auto-saving player data..." << std::endl;
                                             saveCharacterFile(data);

                                             std::cout << "Player data auto-saved to file: " << PATHPLAYERFILE << std::endl;
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

bool SaveManager::isAlreadyDataCreated()
{
    const std::string saveDirectory = PATHSAVES;
     if (!std::filesystem::exists(saveDirectory) || !std::filesystem::is_directory(saveDirectory))
    {
        return false; // Directory does not exist or is not a directory
    }

    return !std::filesystem::is_empty(saveDirectory); // Returns true if the directory is not empty
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
