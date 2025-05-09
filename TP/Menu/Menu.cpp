#include "Menu.hpp"


namespace fs = std::filesystem;

std::string Menu::chooseWorld() {
    std::vector<std::string> worldNames;

    std::cout << "=== Menu de sélection de monde ===\n";

    // Parcourt les sous-dossiers
    for (const auto& entry : fs::directory_iterator(PATHSAVES)) {
        if (entry.is_directory()) {
            std::string worldName = entry.path().filename().string();
            worldNames.push_back(worldName);
        }
    }

    if (worldNames.empty()) {
        std::cerr << "Aucun monde trouvé dans 'saves/'\n";
        exit(1);
    }

    // Affiche les choix
    for (size_t i = 0; i < worldNames.size(); ++i) {
        std::cout << i + 1 << ". " << worldNames[i] << "\n";
    }

    // Demande de sélection
    size_t choice = 0;
    while (true) {
        std::cout << "Sélectionnez un monde (1-" << worldNames.size() << "): ";
        std::cin >> choice;

        if (choice >= 1 && choice <= worldNames.size()) {
            break;
        } else {
            std::cout << "Choix invalide. Réessayez.\n";
        }
    }

    return PATHSAVES + "/" + worldNames[choice - 1];
}

std::string Menu::createWorld() {
    std::string worldName;
    std::cout << "=== Création d'un nouveau monde ===\n";
    std::cout << "Entrez un nom pour le nouveau monde : ";
    std::cin >> worldName;

    std::string savePath = PATHSAVES + worldName;

    // Vérifie si le dossier existe déjà
    if (std::filesystem::exists(savePath)) {
        std::cerr << "Erreur : un monde avec ce nom existe déjà.\n";
    }

    // Crée le dossier
    try {
        std::filesystem::create_directories(savePath);
        std::cout << "Monde '" << worldName << "' créé avec succès dans : " << savePath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de la création du monde : " << e.what() << "\n";
    }
    return savePath;
}


int Menu::chooseLoadOrNewWorld() {
    int choice;
    std::cout << "=== Menu de sélection ===\n";
    std::cout << "1. Créer un nouveau monde\n";
    std::cout << "2. Charger un monde existant\n";
    std::cout << "Choisissez une option (1 ou 2) : ";
    std::cin >> choice;

    return choice;
}