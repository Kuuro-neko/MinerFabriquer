#include "Character.hpp"

#include <iostream>

using namespace std;

Character::Character(Transform transform, Camera *camera, MeshObject *mesh = nullptr, Texture *texture = nullptr )
        : SceneNode(transform, mesh, texture), camera(camera) {
    speed = 2.5;
    camera->setPosition(transform.m_translation + CAMERA_POSITION_RELATIVE_TO_PLAYER);
    inventory = new Inventory();
}

void Character::move(glm::vec3 direction) {
    translate(direction * speed);
    camera->setPosition(getWorldPosition() + CAMERA_POSITION_RELATIVE_TO_PLAYER);
}

/**
 * \brief fonction qui réalise l'action en fonction de la touche détectée
 * @param key
 */
void Character::listenAction(float dt, GLFWwindow *window, VoxelChunk &chunkActuel, BlocDatabase &database) {
    update(dt);
    glm::vec3 cameraFrontNoUp = camera->getRotation() * VEC_FRONT;
    cameraFrontNoUp.y = 0.f;
    cameraFrontNoUp = normalize(cameraFrontNoUp);
    glm::vec3 cameraRightNoUp = camera->getRotation() * VEC_RIGHT;
    cameraRightNoUp.y = 0.f;
    cameraRightNoUp = normalize(cameraRightNoUp);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        move(cameraFrontNoUp * dt * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        move(cameraFrontNoUp * -dt * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        move(cameraRightNoUp * dt * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        move(cameraRightNoUp * -dt * speed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        move(glm::vec3(0.f, -dt * speed, 0.f));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        move(glm::vec3(0.f, dt * speed, 0.f));

//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
//        std::cout << "inventaire" << std::endl;
//    }



    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        breakBlock(chunkActuel, database); //on fait un coup de pioche

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        putBlock(chunkActuel, database); //on pose un bloc

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        setSelectedBlock(chunkActuel, database); //on définit le bloc sélectionné dans l'inventaire
}

/**
 * @brief fonction qui renvoie la liste des blocs intersectés par le rayon partant de la caméra
 * 
 * @param chunkActuel 
 * @param database 
 * @return std::vector<glm::vec3> 
 */
std::vector<glm::vec3> Character::getIntersectedBlocks(VoxelChunk &chunkActuel, BlocDatabase &database) {
    std::vector<glm::vec3> intersectedBlocks;

    glm::vec3 directionNormalized = normalize(camera->getRotation() * VEC_FRONT);
    Ray rayon(camera->getPosition(), directionNormalized);

    // on parcourt tous les blocs du chunk actuel
    for (int x = 0; x < chunkActuel.m_sizeX; x++) {
        for (int y = 0; y < chunkActuel.m_sizeY; y++) {
            for (int z = 0; z < chunkActuel.m_sizeZ; z++) {
                int idBloc = chunkActuel.m_cubes[x][y][z];
                if (database.isAir(idBloc)) continue; // si c'est de l'air on skip
                glm::vec3 blocPosition = chunkActuel.getWorldPosition() + glm::vec3(x, y, z); //on récupère la position du bloc -> chunckTransform + position du bloc
                if (rayon.rayIntersectsAABB(rayon, blocPosition, blocPosition + glm::vec3(1.f), maxInteractionDistance)) { //on vérifie si le rayon intersecte le bloc
                    intersectedBlocks.push_back(blocPosition); 
                }
            }
        }
    }
    return intersectedBlocks;
}

/**
 * @brief fonction qui renvoie le bloc le plus proche de la caméra
 * 
 * @param intersectedBlocks 
 * @return glm::vec3 
 */
glm::vec3 Character::getClosestBlock(const std::vector<glm::vec3>& intersectedBlocks) {
    glm::vec3 closestBlock = intersectedBlocks[0];
    float minDistance = glm::distance(camera->getPosition(), closestBlock);

    for (int i = 1; i < intersectedBlocks.size(); i++) {
        float distance = glm::distance(camera->getPosition(), intersectedBlocks[i]);
        if (distance < minDistance) {
            minDistance = distance;
            closestBlock = intersectedBlocks[i];
        }
    }

    return closestBlock;
}


/**
 * @brief fonction qui réalise l'action de casser un bloc
 * 
 * @param chunkActuel 
 * @param database 
 */
void Character::breakBlock(VoxelChunk &chunkActuel, BlocDatabase &database) {
    if (breakCooldown < MAX_BREAK_COOLDOWN) {
        return;
    }
    glm::vec3 directionNormalized = normalize(camera->getRotation() * VEC_FRONT);
    Ray rayon(camera->getPosition(), directionNormalized);
    glm::vec3 rayDirection = normalize(rayon.direction);

    //ETAPE 1 : liste des blocs intersectés
    std::vector<glm::vec3> blocsIntersectes = getIntersectedBlocks(chunkActuel, database);

    //ETAPE 2 : on casse le bloc le plus proche
    // on parcourt la liste des blocs intersectés par le rayon
    if (blocsIntersectes.size() > 0) {
        //on récupère le bloc le plus proche
        glm::vec3 blocPlusProche = getClosestBlock(blocsIntersectes);

        //on casse le bloc le plus proche -> on remplace le bloc par de l'air
        // on affiche le type de bloc cassé
        int idBlocCasse = chunkActuel.playerRemoveBlock(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
        if (idBlocCasse == -1) {
            return;
        }
        // on ajoute l'item dans l'inventaire
        std::cout << "Bloc cassé : " << database.getBloc(idBlocCasse)->name << std::endl;
        //on ajoute l'item dans l'inventaire
        ItemStack item = ItemStack(idBlocCasse, 1);
        inventory->addItem(item);
        inventory->printInventory();
        resetBreakCooldown();
    }
}

/**
 * @brief fonction qui réalise l'action de poser un bloc
 * 
 * @param chunkActuel 
 * @param database 
 */
void Character::putBlock(VoxelChunk &chunkActuel, BlocDatabase &database) {
    if (placeCooldown < MAX_PLACE_COOLDOWN) {
        return;
    }
    if (inventory->getItems().size() == 0) {
        std::cout << "Inventaire vide" << std::endl;
        return;
    }
    ItemStack *item = inventory->getSelectedItem();
    if (item == nullptr) {
        std::cout << "Aucun item sélectionné" << std::endl;
        return;
    }
    if (item->getQuantity() <= 0) {
        std::cout << "Quantité d'item nulle" << std::endl;
        return;
    }
    glm::vec3 directionNormalized = normalize(camera->getRotation() * VEC_FRONT);
    Ray rayon(camera->getPosition(), directionNormalized);
    glm::vec3 rayDirection = normalize(rayon.direction);

    //liste des blocs intersectés
    std::vector<glm::vec3> blocsIntersectes;
    std::vector<int> facesIntersectees;

    //ETAPE 1 : on parcourt tous les blocs du chunk actuel
    for (int x = 0; x < chunkActuel.m_sizeX; x++) {
        for (int y = 0; y < chunkActuel.m_sizeY; y++) {
            for (int z = 0; z < chunkActuel.m_sizeZ; z++) {
                //on récupère l'id du bloc via la database
                int idBloc = chunkActuel.m_cubes[x][y][z];
                if (database.isAir(idBloc)) continue; // si c'est de l'air on skip

                //on récupère la position du bloc -> chunckTransform + position du bloc
                glm::vec3 blocPosition = chunkActuel.getWorldPosition() + glm::vec3(x, y, z);


                //on vérifie si le rayon intersecte le bloc
                int faceIntersectee = rayon.rayIntersectsAABBFace(rayon, blocPosition, blocPosition + glm::vec3(1.f), maxInteractionDistance);
                if (faceIntersectee != -1) {
                    blocsIntersectes.push_back(blocPosition); // on ajoute le bloc dans la liste des éléments intersecté
                    facesIntersectees.push_back(faceIntersectee); // on ajoute la face intersectée
                }
            }
        }
    }
    //ETAPE 2: on pose le bloc sur la position adjacente au plus proche
    if (blocsIntersectes.size() > 0) {
        //on récupère le bloc le plus proche
        glm::vec3 blocPlusProche = blocsIntersectes[0];
        int facePlusProche = facesIntersectees[0];
        float distanceMin = glm::distance(camera->getPosition(), blocPlusProche);
        for (int i = 1; i < blocsIntersectes.size(); i++) {
            float distance = glm::distance(camera->getPosition(), blocsIntersectes[i]);
            if (distance < distanceMin) {
                distanceMin = distance;
                blocPlusProche = blocsIntersectes[i];
                facePlusProche = facesIntersectees[i];
            }
        }
        switch (facePlusProche) {
            case BLOC_LEFT:
                blocPlusProche.x -= 1;
                break;
            case BLOC_RIGHT:
                blocPlusProche.x += 1;
                break;
            case BLOC_BOTTOM:
                blocPlusProche.y += 1;
                break;
            case BLOC_TOP:
                blocPlusProche.y -= 1;
                break;
            case BLOC_FRONT:
                blocPlusProche.z += 1;
                break;
            case BLOC_BACK:
                blocPlusProche.z -= 1;
                break;
        }
        
        if (chunkActuel.setBloc(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z, item->getItemId())) {
            std::cout << "Bloc placed : " << database.getBloc(item->getItemId())->name << std::endl;
            inventory->removeItem(item->getItemId(), 1);
            inventory->printInventory();
            resetPlaceCooldown();
        }
    }
}

/**
 * @brief fonction définit le bloc sélectionné par le joueur dans son inventaire
 * 
 * @param chunkActuel 
 * @param database 
 */
void Character::setSelectedBlock(VoxelChunk &chunkActuel, BlocDatabase &database) {
    glm::vec3 directionNormalized = normalize(camera->getRotation() * VEC_FRONT);
    Ray rayon(camera->getPosition(), directionNormalized);
    glm::vec3 rayDirection = normalize(rayon.direction);

    //ETAPE 1 : liste des blocs intersectés
    std::vector<glm::vec3> blocsIntersectes = getIntersectedBlocks(chunkActuel, database);

    //ETAPE 2 : on ramasse le bloc le plus proche
    // on parcourt la liste des blocs intersectés par le rayon
    if (blocsIntersectes.size() > 0) {
        //on récupère le bloc le plus proche
        glm::vec3 blocPlusProche = getClosestBlock(blocsIntersectes);

        //on ramasse le bloc le plus proche -> on remplace le bloc par de l'air
        // on affiche le type de bloc ramassé
        int idBlocSelectionne = chunkActuel.getBloc(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
        if (idBlocSelectionne == -1) {
            return;
        }
        std::cout << "Bloc sélectionné : " << database.getBloc(idBlocSelectionne)->name << std::endl;
        inventory->tryToSelectItem(database.getBloc(idBlocSelectionne)->id);
        inventory->printInventory();
    }
}


void Character::scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    // Example: Adjust inventory selection based on scroll
    if (yOffset > 0) {
        std::cout << "Scroll up" << std::endl;
        inventory->scrollSelectedItem(1);
    } else if (yOffset < 0) {
        std::cout << "Scroll down" << std::endl;
        inventory->scrollSelectedItem(-1);
    }
    inventory->printInventory();
}