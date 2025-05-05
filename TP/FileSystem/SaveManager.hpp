#include <string>
#include "TP/Scene/World.hpp"
#include <TP/Character/Character.hpp>
#include <thread>
#include <atomic>
#include <chrono>

class SaveManager
{

protected:
    static SaveManager *instance;

    SaveManager() = default;

    // destructeur
    ~SaveManager()
    {
        // Stop the auto-save thread if it's running
        stopAutoSave();
    }

    // Disable copy constructor and assignment operator

public:
    std::atomic<bool> autoSaveRunning{false};
    std::thread autoSaveThread;

    // Disable copy constructor and assignment operator
    SaveManager(const SaveManager &) = delete;
    SaveManager &operator=(const SaveManager &) = delete;
    SaveManager(SaveManager &&) = delete;

    // Singleton instance -> if instance is null, create a new instance else return the existing one
    static SaveManager &getInstance();

    void saveWorld(World &world, const std::string &filename);
    void loadWorld(World &world, const std::string &filename);

    //-----player data---------//
    // TODO : toute les X secondes/minutes on va sauvegarder le fichier les données  du joueurs
    // Si c'est la première fois qu'on lance un nouveau monde, on va créer un fichier de sauvegarde
    // et on va y mettre les données du joueur par défault et on va le sauvegarder au fur et à mesure

    bool isPlayerDataFileExist(const std::string &filename);

    //return if PATHSAVES contains a folder
    bool isSaveFolderEmpty();

    // load the player data from the file -> default values if the file does not exist, file's values if it does
    void loadPlayerData(Character &character, const std::string &filename);

   
    // character data save
    void saveCharacterFile(Character &data);

    // function that start the auto save thread and save the player data every X seconds
    void startAutoSave(Character &data);
    void stopAutoSave();
    // Functions used to create the folder name YYYY-MM-DD based on the timestamp
    std::string getDate(long timestamp);
    long getTimestamp();
    std::string generateSaveFolderPath();

    std::string getMostRecentSaveFolder();
};