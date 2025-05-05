#include "SaveManager.hpp"
#include <fstream>
#include <iostream>

// if the instance is null, create it else return the existing one -> thread safe
SaveManager &SaveManager::getInstance()
{
    static SaveManager instance; // local static variable
    return instance;
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
