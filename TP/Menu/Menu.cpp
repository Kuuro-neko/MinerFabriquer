#include "Menu.hpp"


namespace fs = std::filesystem;

std::string Menu::chooseWorld() {
    std::vector<std::string> worldNames;

    std::cout << "=== Menu of worlds selection ===\n";

    // Parcourt les sous-dossiers
    for (const auto& entry : fs::directory_iterator(PATHSAVES)) {
        if (entry.is_directory()) {
            std::string worldName = entry.path().filename().string();
            worldNames.push_back(worldName);
        }
    }

    if (worldNames.empty()) {
        std::cerr << "No wordls founds in 'saves/'\n";
        exit(1);
    }

    // Affiche les choix
    for (size_t i = 0; i < worldNames.size(); ++i) {
        std::cout << i + 1 << ". " << worldNames[i] << "\n";
    }

    // Demande de sélection
    size_t choice = 0;
    while (true) {
        std::cout << "Select a world (1-" << worldNames.size() << "): ";
        std::cin >> choice;

        if (choice >= 1 && choice <= worldNames.size()) {
            break;
        } else {
            std::cout << "Wrong input. Retry agin.\n";
        }
    }

    return PATHSAVES  + worldNames[choice - 1];
}

std::string Menu::createWorld() {
    std::string worldName;
    std::cout << "=== Creation of a new world ===\n";
    std::cout << "Enter the new world name : ";
    std::cin >> worldName;

    std::string savePath = PATHSAVES + worldName;

    // Vérifie si le dossier existe déjà
    if (std::filesystem::exists(savePath)) {
        std::cerr << "Error : name already exists .\n";
    }

    // Crée le dossier
    try {
        std::filesystem::create_directories(savePath);
    } catch (const std::exception& e) {
        std::cerr << "Error when trying to create the new world save : " << e.what() << "\n";
    }
    return savePath;
}

std::string Menu::askSeed() {
    std::string seedStr;
    int seed;
    std::cout << "Enter the seed : ";
    std::cin >> seedStr;

    if (seedStr.empty()) {
        seedStr = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }

    return seedStr;
}


int Menu::chooseLoadOrNewWorld() {
    int choice;
    std::cout << "=== World Selection ===\n";
    std::cout << "1. Create a new world\n";
    std::cout << "2. Load an existing world \n";
    std::cout << "Choose an option (1 or 2) : ";
    std::cin >> choice;

    return choice;
}