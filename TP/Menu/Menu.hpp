#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include "Defines.hpp"


class Menu {

public:

    std::string chooseWorld();
    static int chooseLoadOrNewWorld();
    static std::string createWorld();
};

