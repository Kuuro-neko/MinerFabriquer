#include <string>
#include "TP/Scene/World.hpp"

class SaveManager
{

protected:
    static SaveManager *instance;

    SaveManager() = default;
    ~SaveManager() = default;

    // Disable copy constructor and assignment operator

public:
    // Disable copy constructor and assignment operator
    SaveManager(const SaveManager &) = delete;
    SaveManager &operator=(const SaveManager &) = delete;
    SaveManager(SaveManager &&) = delete;

    // Singleton instance -> if instance is null, create a new instance else return the existing one
    static SaveManager &getInstance();



    void saveWorld(World &world, const std::string &filename);
    void loadWorld(World &world, const std::string &filename);

    //-----player data---------//
    //TODO : toute les X secondes/minutes on va sauvegarder le fichier les données  du joueurs
    // Si c'est la première fois qu'on lance un nouveau monde, on va créer un fichier de sauvegarde
    // et on va y mettre les données du joueur par défault et on va le sauvegarder au fur et à mesure



    struct playerData{
        unsigned char gamemode;
        unsigned char prev;
        float position[3];
    };

    bool isPlayerDataFileExist(const std::string &filename);

    //load the player data from the file -> default values if the file does not exist, file's values if it does
    void loadPlayerData(Character &character, const std::string &filename);

 
};