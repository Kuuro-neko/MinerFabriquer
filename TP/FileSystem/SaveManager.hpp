#include <string>
#include "TP/Scene/World.hpp"
#include <TP/Character/Character.hpp>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>

class SaveManager {

private:
    World *world = nullptr; // Pointeur vers l'instance de World
    Character *character = nullptr; // Pointeur vers l'instance de Character

protected:
    static SaveManager *instance;

    SaveManager() = default;

    // destructeur
    ~SaveManager() {
        // Stop the auto-save thread if it's running

        stopAutoSave();
    }

public:
    std::atomic<bool> autoSaveRunning{false};
    std::thread autoSaveThread;

    // Disable copy constructor and assignment operator
    SaveManager(const SaveManager &) = delete;

    SaveManager &operator=(const SaveManager &) = delete;

    SaveManager(SaveManager &&) = delete;

    // Singleton instance -> if instance is null, create a new instance else return the existing one
    static SaveManager &getInstance();

    //-----player data---------//

    // return if PATHSAVES contains a folder
    bool isSaveFolderEmpty();

    // load the player data from the file -> default values if the file does not exist, file's values if it does
    void loadPlayerData();

    // character data save
    void saveCharacterFile();

    // function that start the auto save thread and save the player data every X seconds
    void startAutoSave();

    void stopAutoSave();

    void createPlayerDataFile();

    //-----world data---------//

    // the way the world is saved ->
    // header contaning the offset and the lneght of each region data

    // a region is composed of 32 x 32 chunk columns
    // each chunk column is composed of 8 chunks, a heightmap and defined by its coordinates
    // each chunk is composed of 16 x 16 x 16 ID block [octet], a lightmap [16x16x16] octet

    // for the data that we know it,s going to change we go for a vector
    // for the data that we know it,s not going to change we go for a static array


    struct ChunkEntry {
        std::vector<int8_t> blocksID; // 16×16×16 of 1 octets = 4096 octets
        std::vector<int8_t> lightmap; // 16×16×16 of 1 octets   = 4096 octets
    };

    struct ChunkColumnEntry {
        int32_t worldX; // X global coordinate
        int32_t worldZ; // Z global coordinate

        int32_t heightmap[16][16]; // 1024 * 4 = 4096 octets

        ChunkEntry chunks[8]; // 8 niveaux verticaux

        uint32_t offset; // Region position in the file
        uint32_t length; // lenght of the region's data in the file
    };

    void setWorld(World *worldInstance);
    void setCharacter(Character *characterInstance);

    void saveWorldFile();

    std::vector<ChunkColumnEntry> loadWorldFile();

    std::vector<ChunkColumnEntry> readWorldFile(std::ifstream &in);


    bool isDataFolderContainsOtherFolder();

    std::string saveFolderPath;

    inline void setSaveFolderPath(const std::string &path) {
        saveFolderPath = path;
    }
    inline std::string getSaveFolderPath() {
        return saveFolderPath;
    }


};