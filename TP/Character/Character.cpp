#include "Character.hpp"
#include "TP/Scene/Renderer.hpp"


#include <iostream>
#include <optional>

using namespace std;

Character::Character(Transform transform, Camera *camera, MeshObject *mesh = nullptr, Texture *texture = nullptr)
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
void Character::listenAction(float dt, GLFWwindow *window, World &chunkActuel, BlocDatabase &database) {
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


    updateClosestBlock(chunkActuel, database); //on met à jour le bloc le plus proche

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        breakBlock(chunkActuel, database); //on fait un coup de pioche
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        putBlock(chunkActuel, database); //on pose un bloc
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        setSelectedBlock(chunkActuel, database); //on sélectionne un bloc
    }
}

void Character::updateClosestBlock(World& world, BlocDatabase& db) {
    Ray ray(camera->getPosition(), glm::normalize(camera->getRotation() * VEC_FRONT));
    VoxelChunk* chunk = world.getChunkContaining(camera->getPosition());
    glm::vec3 origin = chunk->getWorldPosition();

    float minDist = maxInteractionDistance;
    intersection = false;

    // Pour tout bloc du chunk
    for (int x = 0; x < chunk->m_sizeX; ++x) {
        for (int y = 0; y < chunk->m_sizeY; ++y) {
            for (int z = 0; z < chunk->m_sizeZ; ++z) {
                int id = chunk->m_cubes[x][y][z];
                if (db.isAir(id)) continue;  // si c'est de l'air on skip

                glm::vec3 pos = origin + glm::vec3(x, y, z); //on récupère la position du bloc -> chunckTransform + position du bloc
                
                // on vérifie si le rayon intersecte le bloc
                int face = ray.rayIntersectsAABBFace(ray, pos, pos + glm::vec3(1.f), maxInteractionDistance);
                if (face == -1) continue; // si pas d'intersection on skip
                
                float dist = glm::distance(camera->getPosition(), pos);
                if (dist < minDist) { // on vérifie si le bloc est plus proche que le précédent trouvé
                    intersection = true;
                    blocPlusProche = pos;
                    facePlusProche = face;
                    minDist = dist;
                }
            }
        }
    }
    if (intersection) {
        // set highlight
        renderer->setHighlight(glm::vec3(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z));
    } else {
        renderer->disableHighlight();
    }
}


/**
 * @brief fonction qui réalise l'action de casser un bloc
 * 
 * @param chunkActuel 
 * @param database 
 */
void Character::breakBlock(World &chunkActuel, BlocDatabase &database) {
    if (!intersection || breakCooldown < MAX_BREAK_COOLDOWN) return;

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

/**
 * @brief fonction qui réalise l'action de poser un bloc
 * 
 * @param chunkActuel 
 * @param database 
 */
void Character::putBlock(World &chunkActuel, BlocDatabase &database) {
    if (!intersection || placeCooldown < MAX_PLACE_COOLDOWN) return;
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
    glm::vec3 position = glm::vec3(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
    switch (facePlusProche) {
        case BLOC_LEFT:
            position.x -= 1;
            break;
        case BLOC_RIGHT:
            position.x += 1;
            break;
        case BLOC_BOTTOM:
            position.y += 1;
            break;
        case BLOC_TOP:
            position.y -= 1;
            break;
        case BLOC_FRONT:
            position.z += 1;
            break;
        case BLOC_BACK:
            position.z -= 1;
            break;
    }
    
    if (chunkActuel.setBloc(position.x, position.y, position.z, item->getItemId())) {
        std::cout << "Bloc placed : " << database.getBloc(item->getItemId())->name << std::endl;
        inventory->removeItem(item->getItemId(), 1);
        inventory->printInventory();
        resetPlaceCooldown();
    }
}

/**
 * @brief fonction définit le bloc sélectionné par le joueur dans son inventaire
 * 
 * @param chunkActuel 
 * @param database 
 */
void Character::setSelectedBlock(World &chunkActuel, BlocDatabase &database) {
    if (!intersection) return;
    // on définit le bloc sélectionné par le joueur dans son inventaire
    int idBlocSelectionne = chunkActuel.getBloc(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
    if (idBlocSelectionne == -1) {
        return;
    }
    std::cout << "Bloc sélectionné : " << database.getBloc(idBlocSelectionne)->name << std::endl;
    inventory->tryToSelectItem(database.getBloc(idBlocSelectionne)->id);
    inventory->printInventory();
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