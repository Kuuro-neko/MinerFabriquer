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

    // TODO
    void saveWorld(World &world, const std::string &filename);
    void loadWorld(World &world, const std::string &filename);
};