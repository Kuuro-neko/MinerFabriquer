#include "SaveManager.hpp"
#include <fstream>
#include <iostream>
#include <TP/Character/Character.hpp>

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
    // the file is not created yet -> we create it
    if (!isPlayerDataFileExist(filename))
    {
        std::cout << "Player data file does not exist." << std::endl;
        std::cout << "Creating a new player data file." << std::endl;
        std::ofstream file(filename);
        std::ofstream ofs(filename, std::ios::binary);

        // Default values (that's on default values)
        unsigned char gamemode = character.getGamemode();
        unsigned char prev = character.GetprevGamemode();
        float position[3] = {
            character.getWorldPosition().x,
            character.getWorldPosition().y,
            character.getWorldPosition().z
        };

        // Write the data to the file
        ofs.write(reinterpret_cast<const char *>(&gamemode), sizeof(gamemode));
        ofs.write(reinterpret_cast<const char *>(&prev), sizeof(prev));
        ofs.write(reinterpret_cast<const char *>(position), sizeof(position));
        ofs.close();
        std::cout << "Player data file created and filled with default values." << std::endl;
        
    }
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
