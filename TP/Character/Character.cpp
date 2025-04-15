#include "Character.hpp"
#include <iostream>
#include <optional>

using namespace std;

Character::Character(Transform transform, Camera *camera, World *world, MeshObject *mesh, Texture *texture)
        : SceneNode(transform, mesh, texture), camera(camera), m_world(world), size() , velocity(){
    camera->setPosition(transform.m_translation + CAMERA_POSITION_RELATIVE_TO_PLAYER);
    inventory = new Inventory();
    updateBoundingBox();
    //on setHightlight la bounding box
    renderer->setHighlight(getMinBoundingBox());
}

void Character::move(glm::vec3 direction) {
    translate(direction * speed);
    camera->setPosition(getWorldPosition() + CAMERA_POSITION_RELATIVE_TO_PLAYER);
}

/**
 * \brief fonction qui réalise l'action en fonction de la touche détectée
 * @param key
 */

void Character::listenAction(float dt, GLFWwindow *window, BlocDatabase &database) {
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


    updateClosestBlock(database); //on met à jour le bloc le plus proche

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && breakCooldown >= MAX_BREAK_COOLDOWN) {
        breakBlock(database); // on fait un coup de pioche
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && placeCooldown >= MAX_PLACE_COOLDOWN) {
        putBlock(database); //on pose un bloc
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        std::cout << "Selected bloc is at " << blocPlusProche.x << ", " << blocPlusProche.y << ", " << blocPlusProche.z
                  << std::endl;
        setSelectedBlock(database); //on sélectionne un bloc
    }
}

void Character::updateClosestBlock(BlocDatabase &db) {
    Ray ray(camera->getPosition(), glm::normalize(camera->getRotation() * VEC_FRONT));
    std::vector<VoxelChunk *> chunks = m_world->getIntersectedChunks(ray, maxInteractionDistance);
    intersection = false;
    if (chunks.empty()) {
        renderer->disableHighlight();
        return;
    }
    std::vector<glm::vec3> origins;
    for (auto &chunk: chunks) {
        origins.push_back(chunk->getWorldPosition());
    }

    float minDist = maxInteractionDistance;

    // Pour tout bloc des chunks touchés par le rayon
    for (int x = 0; x < chunks[0]->m_sizeX; ++x) {
        for (int y = 0; y < chunks[0]->m_sizeY; ++y) {
            for (int z = 0; z < chunks[0]->m_sizeZ; ++z) {
                for (int i = 0; i < chunks.size(); ++i) {
                    int id = chunks[i]->m_cubes[x][y][z];
                    if (db.isAir(id)) continue;  // si c'est de l'air on skip
                    glm::vec3 pos = origins[i] + glm::vec3(x, y,
                                                           z); //on récupère la position du bloc -> chunckTransform + position du bloc

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
void Character::breakBlock(BlocDatabase &database) {
    if (!intersection || breakCooldown < MAX_BREAK_COOLDOWN) return;

    //on casse le bloc le plus proche -> on remplace le bloc par de l'air
    // on affiche le type de bloc cassé
    int idBlocCasse = m_world->playerRemoveBlock(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
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
void Character::putBlock(BlocDatabase &database) {
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

    if (m_world->setBloc(position.x, position.y, position.z, item->getItemId())) {
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
void Character::setSelectedBlock(BlocDatabase &database) {
    if (!intersection) return;
    // on définit le bloc sélectionné par le joueur dans son inventaire
    int idBlocSelectionne = m_world->getBloc(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
    if (idBlocSelectionne == -1) {
        return;
    }
    std::cout << "Bloc sélectionné : " << database.getBloc(idBlocSelectionne)->name << std::endl;
    inventory->tryToSelectItem(database.getBloc(idBlocSelectionne)->id);
    inventory->printInventory();
}


void Character::scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
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

/**
 * \brief fonction qui met à jour le personnage et sa physique
 * @param dt
 */
void Character::update(float dt) {
    if (breakCooldown < MAX_BREAK_COOLDOWN) {
        breakCooldown += dt;
    }
    if (placeCooldown < MAX_PLACE_COOLDOWN) {
        placeCooldown += dt;
    }
    updateBoundingBox();
    renderer->setHighlight(getMinBoundingBox());
}

/**
 * \brief fonction qui met à jour la bounding box du personnage
 */
void Character::updateBoundingBox() {
    boundingBox.clear();
    glm::vec3 position = getWorldPosition();
    boundingBox.push_back(position + glm::vec3(-size, -size, -size));
    boundingBox.push_back(position + glm::vec3(size, -size, -size));
    boundingBox.push_back(position + glm::vec3(size, -size, size));
    boundingBox.push_back(position + glm::vec3(-size, -size, size));
    boundingBox.push_back(position + glm::vec3(-size, size, -size));
    boundingBox.push_back(position + glm::vec3(size, size, -size));
    boundingBox.push_back(position + glm::vec3(size, size, size));
    boundingBox.push_back(position + glm::vec3(-size, size, size));
}

glm::vec3 Character::getMinBoundingBox() {
    glm::vec3 min = boundingBox[0];
    for (int i = 1; i < boundingBox.size(); ++i) {
        min = glm::min(min, boundingBox[i]);
    }
    return min;
}
glm::vec3 Character::getMaxBoundingBox() {
    glm::vec3 max = boundingBox[0];
    for (int i = 1; i < boundingBox.size(); ++i) {
        max = glm::max(max, boundingBox[i]);
    }
    return max;
}

void Character::drawBoundingBox(GLuint programID) {
    //use renderer to draw the bounding box

    glUseProgram(programID);
    renderer->drawWireframeCube(
            glm::vec3(size, size, size),
            camera->getViewMatrix(),
            camera->getProjectionMatrix()
    );
}

glm::vec3 Character::getSize() {
    return glm::vec3(size, size, size);
}

