#include <string>
#include "TP/Scene/World.hpp"
#include <TP/Character/Character.hpp>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>

#include <TP/Scene/WorldGenerator.hpp>

class SaveManager
{

private:
    World *world = nullptr;         // Pointeur vers l'instance de World
    Character *character = nullptr; // Pointeur vers l'instance de Character

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

    // Header structure for player data file
    struct PlayerDataHeader
    {
        uint32_t characterDataOffset;
        uint32_t characterDataSize;
        uint32_t inventoryOffset;
        uint32_t inventorySize;
        uint32_t totalFileSize;
    };

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

    //-----seed data---------//

    // Header structure for seed file
    struct SeedHeader
    {
        uint32_t seedDataOffset;
        uint32_t seedDataSize;
        uint32_t totalFileSize;
    };

    void createSeedFile();
    void saveSeedFile();
    void readSeedFile();

    //-----world data---------//

    // Header structure for world file
    struct WorldFileHeader
    {
        uint32_t columnCount;        // Number of chunk columns in the file
        uint32_t columnsTableOffset; // Offset to the table of column entries
        uint32_t dataStartOffset;    // Offset to the start of chunk data
        uint32_t totalFileSize;      // Total size of the file
    };

    // Column entry in the header table
    struct ColumnTableEntry
    {
        int32_t worldX;      // X global coordinate
        int32_t worldZ;      // Z global coordinate
        uint32_t dataOffset; // Offset to column data
        uint32_t dataLength; // Length of column data
    };

    struct ChunkEntry
    {
        std::vector<int8_t> blocksID; // 16×16×16 of 1 octets = 4096 octets
        std::vector<int8_t> lightmap; // 16×16×16 of 1 octets   = 4096 octets
    };

    struct ChunkColumnEntry
    {
        int32_t worldX;            // X global coordinate
        int32_t worldZ;            // Z global coordinate
        int32_t heightmap[16][16]; // 1024 * 4 = 4096 octets
        ChunkEntry chunks[8];      // 8 niveaux verticaux
        uint32_t offset;           // Region position in the file
        uint32_t length;           // lenght of the region's data in the file
    };

    void setWorld(World *worldInstance);
    void setCharacter(Character *characterInstance);

    void saveWorldFile();
    std::vector<ChunkColumnEntry> loadWorldFile();
    std::vector<ChunkColumnEntry> readWorldFile(std::ifstream &in);

    bool isDataFolderContainsOtherFolder();

    std::string saveFolderPath;

    inline void setSaveFolderPath(const std::string &path)
    {
        saveFolderPath = path;
    }

    inline std::string getSaveFolderPath()
    {
        return saveFolderPath;
    }
};