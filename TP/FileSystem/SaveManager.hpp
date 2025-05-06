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
    void loadPlayerData(Character &character);

    // character data save
    void saveCharacterFile(Character &data);

    // function that start the auto save thread and save the player data every X seconds
    void startAutoSave(Character &data);
    void stopAutoSave();
    // Functions used to create/get the folder name YYYY-MM-DD based on the timestamp to get the most recent
    std::string getDate(long timestamp);
    long getTimestamp();
    std::string generateSaveFolderPath();
    std::string getMostRecentSaveFolder();

    //-----world data---------//

    // the way the world is saved ->
    // header contaning the offset and the lneght of each region data

    // a region is composed of 32 x 32 chunk columns
    // each chunk column is composed of 8 chunks, a heightmap and defined by its coordinates
    // each chunk is composed of 16 x 16 x 16 ID block [octet], a lightmap [16x16x16] octet

    // for the data that we know it,s going to change we go for a vector
    // for the data that we know it,s not going to change we go for a static array
    struct ChunkColumnEntry
    {
        uint32_t offset; // Region position in the file
        uint32_t length; // lenght of the region's data in the file
    };

    struct Chunk
    {
        std::vector<uint8_t> blocksID; // 16×16×16 of 1 octets = 4096 octets
        std::vector<uint8_t> lightmap; // 16×16×16 of 1 octets   = 4096 octets
    };

    struct ChunkColumn
    {
        int32_t worldX; // X global coordinate
        int32_t worldZ; // Z global coordinate

        int32_t heightmap[16][16]; // 1024 * 4 = 4096 octets

        Chunk chunks[8]; // 8 niveaux verticaux
    };

    void saveWorldFile(World &world);
   
};