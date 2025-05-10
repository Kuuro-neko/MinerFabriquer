#include "Character.hpp"
#include <iostream>
#include <optional>

using namespace std;

std::string gamemodeString(int gamemode)
{
    switch (gamemode)
    {
    case GAMEMODE_CREATIVE:
        return "GAMEMODE_CREATIVE";
    case GAMEMODE_SURVIVAL:
        return "GAMEMODE_SURVIVAL";
    case GAMEMODE_SPECTATOR:
        return "GAMEMODE_SPECTATOR";
    default:
        return "UNKNOWN_GAMEMODE";
    }
}

Character::Character(Transform transform, Camera *camera, World *world, MeshObject *mesh, Texture *texture)
    : SceneNode(transform, mesh, texture), camera(camera), m_world(world), size(), velocity()
{
    camera->setPosition(transform.m_translation + CAMERA_POSITION_RELATIVE_TO_PLAYER);
    inventory = new Inventory();
    updateBoundingBox();
    // Minecraft AABB width : 5/8
    // Minecraft AABB height : 29/32
    // Minecraft AABB height while sneaking : 1.5
    size = glm::vec3(5.f / 8.f, 29.f / 16.f, 5.f / 8.f);
    // on setHightlight la bounding box
}

void Character::move(glm::vec3 direction)
{
    translate(direction);
    camera->setPosition(getWorldPosition() + CAMERA_POSITION_RELATIVE_TO_PLAYER);
}

/**
 * \brief fonction qui réalise les actions en fonction de la touche détectée
 * @param key
 */
void Character::listenAction(float dt)
{

    update(dt);
    glm::vec3 cameraFrontNoUp = camera->getRotation() * VEC_FRONT;
    cameraFrontNoUp.y = 0.f;
    cameraFrontNoUp = normalize(cameraFrontNoUp);
    glm::vec3 cameraRightNoUp = camera->getRotation() * VEC_RIGHT;
    cameraRightNoUp.y = 0.f;
    cameraRightNoUp = normalize(cameraRightNoUp);
    vecteurDirection = glm::vec3(0.f);

    alignWithCamera(cameraFrontNoUp);

    glm::vec3 currentPosition = getWorldPosition();
    int currentBlock = m_world->getBloc(static_cast<int>(currentPosition.x),
                                        static_cast<int>(currentPosition.y),
                                        static_cast<int>(currentPosition.z));

    this->isInWater = (currentBlock == WATER);

    if (gamemode != GAMEMODE_SPECTATOR)
    {
        if (isInWater)
        {
            speed = WATER_SPEED;
            sneakSpeed = WATER_SNEAK_SPEED;
            sprintSpeed = WATER_SPRINT_SPEED;
        }
        else
        {
            speed = DEFAULT_SPEED;
            sneakSpeed = DEFAULT_SNEAK_SPEED;
            sprintSpeed = DEFAULT_SPRINT_SPEED;
        }
    }

    // ==== Movement binds ====
    if (keyInput->isKeybindHeld(keybinds->forward))
        vecteurDirection += cameraFrontNoUp;
    if (keyInput->isKeybindHeld(keybinds->backward))
        vecteurDirection -= cameraFrontNoUp;
    if (keyInput->isKeybindHeld(keybinds->left))
        vecteurDirection += cameraRightNoUp;
    if (keyInput->isKeybindHeld(keybinds->right))
        vecteurDirection -= cameraRightNoUp;
    if (keyInput->isKeybindHeld(keybinds->jump))
        vecteurDirection += VEC_UP;
    
    if (gamemode == GAMEMODE_SPECTATOR || gamemode == GAMEMODE_CREATIVE)
    {
        if (keyInput->isKeybindHeld(keybinds->sneak))
            vecteurDirection -= VEC_UP;
    }

    if (glm::length(vecteurDirection) > 0.01f)
    {
        vecteurDirection = glm::normalize(vecteurDirection); // normalize to not go faster on diagonals
    }


    if (keyInput->isKeybindHeld(keybinds->sneak)  && this->gamemode == GAMEMODE_SURVIVAL)
    {
        vecteurDirection *= this->sneakSpeed * dt;
        this->sneaking = true;
        this->sprinting = false;
    }
    else if (keyInput->isKeybindHeld(keybinds->sprint) && this->gamemode != GAMEMODE_SPECTATOR)
    {
        vecteurDirection *= this->sprintSpeed * dt;
        this->sneaking = false;
        this->sprinting = true;
    }
    else
    {
        vecteurDirection *= this->speed * dt;
        this->sneaking = false;
        this->sprinting = false;
    }

    updateCamera();

    if (keyInput->isKeybindPressed(keybinds->openInventory))
    {
        std::cout << "[Character] Inventaire" << std::endl;
    }

    // if (keyInput->isKeybindHeld(keybinds->sprint)) {
    //     std::cout << "[Character] Sprint" << std::endl;
    // }
    BlocDatabase &db = BlocDatabase::getInstance();

    if (gamemode == GAMEMODE_SPECTATOR)
    {
        targetCubeRenderer->disableHighlight();
    }
    else
    {
        updateClosestBlock(db); // on met à jour le bloc le plus proche

        // ==== Bloc interaction binds ====
        if (keyInput->isKeybindHeld(keybinds->breakBlock) && breakCooldown >= MAX_BREAK_COOLDOWN)
        {
            breakBlock(db); // on fait un coup de pioche
        }

        if (keyInput->isKeybindHeld(keybinds->placeBlock) && placeCooldown >= MAX_PLACE_COOLDOWN)
        {
            putBlock(db); // on pose un bloc
        }

        if (keyInput->isKeybindHeld(keybinds->selectBlock))
        {
            setSelectedBlock(db); // on sélectionne un bloc
        }
    }

    // ==== Debug binds ====
    if (keyInput->isKeybindPressed(keybinds->toggleBoudingBoxes))
    {
        std::cout << "[Character] Toggle bounding boxes" << std::endl;
        displayAABB = !displayAABB;
        shouldToggleDebug = false;
    }

    if (keyInput->isKeybindPressed(keybinds->toggleChunkBorders))
    {
        std::cout << "[Character] Toggle chunk borders (not implemented)" << std::endl;
        shouldToggleDebug = false;
    }

    if (keyInput->isKeybindPressed(keybinds->toggleWireframe))
    {
        std::cout << "[Character] Toggle wireframe (not implemented)" << std::endl;
        shouldToggleDebug = false;
    }

    if (keyInput->isKeybindPressed(keybinds->toggleSpectator))
    {
        this->speed = DEFAULT_SPEED;
        std::cout << "[Character] Toggle spectator mode" << std::endl;
        if (gamemode == GAMEMODE_SPECTATOR)
        {
            gamemode = prevGamemode;
        
        } else {
            prevGamemode = gamemode;
            gamemode = GAMEMODE_SPECTATOR;
        }
        std::cout << "[Character] Set gamemode to " << gamemodeString(gamemode) << std::endl;
        shouldToggleDebug = false;
    }

    if (keyInput->isKeybindPressed(keybinds->toggleCreative))
    {
        this->speed = DEFAULT_SPEED;
        std::cout << "[Character] Toggle creative mode" << std::endl;
        if (gamemode == GAMEMODE_CREATIVE) {
            gamemode = GAMEMODE_SURVIVAL;
        } else {
            gamemode = GAMEMODE_CREATIVE;
        }
        std::cout << "[Character] Set gamemode to " << gamemodeString(gamemode) << std::endl;
        shouldToggleDebug = false;
    }

    if (keyInput->isKeybindPressed(keybinds->reloadChunkMeshes))
    {
        std::cout << "[Character] Reload chunk meshes (not implemented)" << std::endl;
        shouldToggleDebug = false;
    }

    if (keyInput->isKeyReleased(keybinds->getToggleDebug()))
    {
        if (shouldToggleDebug)
        {
            std::cout << "[Character] Toggle debug mode (not implemented)" << std::endl;
        }
        else
        {
            shouldToggleDebug = true;
        }
    }
}

void Character::updateClosestBlock(BlocDatabase &db)
{
    Ray ray(camera->getPosition(), glm::normalize(camera->getRotation() * VEC_FRONT));
    std::vector<VoxelChunk *> chunks = m_world->getIntersectedChunks(ray, maxInteractionDistance);
    intersection = false;
    if (chunks.empty())
    {
        targetCubeRenderer->disableHighlight();
        return;
    }
    std::vector<glm::vec3> origins;
    for (auto &chunk : chunks)
    {
        origins.push_back(chunk->getWorldPosition());
    }

    float minDist = maxInteractionDistance;

    // Pour tout bloc des chunks touchés par le rayon
    for (int x = 0; x < chunks[0]->m_sizeX; ++x)
    {
        for (int y = 0; y < chunks[0]->m_sizeY; ++y)
        {
            for (int z = 0; z < chunks[0]->m_sizeZ; ++z)
            {
                for (int i = 0; i < chunks.size(); ++i)
                {
                    int id = chunks[i]->m_cubes[x][y][z];
                    if (db.isAir(id))
                        continue; // si c'est de l'air on skip
                    glm::vec3 pos = origins[i] + glm::vec3(x, y, z); // on récupère la position du bloc -> chunckTransform + position du bloc

                    // on vérifie si le rayon intersecte le bloc
                    int face = ray.rayIntersectsAABBFace(ray, pos, pos + glm::vec3(1.f), maxInteractionDistance);
                    if (face == -1)
                        continue; // si pas d'intersection on skip

                    float dist = glm::distance(camera->getPosition(), pos);
                    if (dist < minDist)
                    { // on vérifie si le bloc est plus proche que le précédent trouvé
                        intersection = true;
                        blocPlusProche = pos;
                        facePlusProche = face;
                        minDist = dist;
                    }
                }
            }
        }
    }
    if (intersection)
    {
        // set highlight
        targetCubeRenderer->setHighlight(glm::vec3(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z));
    }
    else
    {
        targetCubeRenderer->disableHighlight();
    }
}

/**
 * @brief fonction qui réalise l'action de casser un bloc
 *
 * @param chunkActuel
 * @param database
 */
void Character::breakBlock(BlocDatabase &database)
{
    if (!intersection || breakCooldown < MAX_BREAK_COOLDOWN)
        return;

    // on casse le bloc le plus proche -> on remplace le bloc par de l'air
    //  on affiche le type de bloc cassé
    int idBlocCasse = m_world->playerRemoveBlock(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z, getGamemode());
    if (idBlocCasse == -1)
    {
        return;
    }
    // on ajoute l'item dans l'inventaire
    std::cout << "Bloc cassé : " << database.getBloc(idBlocCasse)->name << std::endl;
    // on ajoute l'item dans l'inventaire
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
void Character::putBlock(BlocDatabase &database)
{
    if (!intersection || placeCooldown < MAX_PLACE_COOLDOWN)
        return;
    if (inventory->getItems().size() == 0)
    {
        std::cout << "Inventaire vide" << std::endl;
        return;
    }
    ItemStack *item = inventory->getSelectedItem();
    if (item == nullptr)
    {
        std::cout << "Aucun item sélectionné" << std::endl;
        return;
    }
    if (item->getQuantity() <= 0)
    {
        std::cout << "Quantité d'item nulle" << std::endl;
        return;
    }
    glm::vec3 position = glm::vec3(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
    switch (facePlusProche) {
        case FACE_EAST:
            position.x -= 1;
            break;
        case FACE_WEST:
            position.x += 1;
            break;
        case FACE_BOTTOM:
            position.y += 1;
            break;
        case FACE_TOP:
            position.y -= 1;
            break;
        case FACE_SOUTH:
            position.z += 1;
            break;
        case FACE_NORTH:
            position.z -= 1;
            break;
    }

    if (m_world->setBloc(position.x, position.y, position.z, item->getItemId()))
    {
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
void Character::setSelectedBlock(BlocDatabase &database)
{
    if (!intersection)
        return;
    // on définit le bloc sélectionné par le joueur dans son inventaire
    int idBlocSelectionne = m_world->getBloc(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
    if (idBlocSelectionne == -1)
    {
        return;
    }
    std::cout << "Bloc sélectionné : " << database.getBloc(idBlocSelectionne)->name << std::endl;
    inventory->tryToSelectItem(database.getBloc(idBlocSelectionne)->id);
    inventory->printInventory();
}

void Character::updateCamera()
{
    if (gamemode == GAMEMODE_SPECTATOR)
    {
        camera->setPlayerMotions(false, false);
    }
    else
    {
        camera->setPlayerMotions(sprinting, sneaking);
    }
}

void Character::scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
    // Example: Adjust inventory selection based on scroll
    switch (this->gamemode)
    {
    case GAMEMODE_CREATIVE:
    case GAMEMODE_SURVIVAL:
        if (yOffset > 0)
        {
            std::cout << "Scroll up" << std::endl;
            inventory->scrollSelectedItem(1);
            if (m_hud != nullptr) {
                m_hud->getBarre()->nextSelectedSlot(1);
            }
        }
        else if (yOffset < 0)
        {
            std::cout << "Scroll down" << std::endl;
            inventory->scrollSelectedItem(-1);
            if (m_hud != nullptr) {
                m_hud->getBarre()->nextSelectedSlot(-1);
            }
        }
        inventory->printInventory();
        
        break;
    case GAMEMODE_SPECTATOR:
        if (yOffset > 0)
        {
            this->speed += 0.1f;
        }
        else if (yOffset < 0)
        {
            this->speed -= 0.1f;
        }
        break;
    }
}

/**
 * \brief fonction qui met à jour le personnage et sa physique
 * @param dt
 */
void Character::update(float dt)
{
    if (breakCooldown < MAX_BREAK_COOLDOWN)
    {
        breakCooldown += dt;
    }
    if (placeCooldown < MAX_PLACE_COOLDOWN)
    {
        placeCooldown += dt;
    }
    updateBoundingBox();
    AABBRenderer->setHighlight(getMinBoundingBox());
}

/**
 * \brief fonction qui met à jour la bounding box du personnage
 */
void Character::updateBoundingBox()
{
    boundingBox.clear();
    glm::vec3 position = getWorldPosition();
    boundingBox.push_back(position + glm::vec3(-size.x / 2, -size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, -size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(-size.x / 2, size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(-size.x / 2, -size.y / 2, size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, -size.y / 2, size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, size.y / 2, size.z / 2));
    boundingBox.push_back(position + glm::vec3(-size.x / 2, size.y / 2, size.z / 2));
}

glm::vec3 Character::getMinBoundingBox()
{
    glm::vec3 min = boundingBox[0];
    for (int i = 1; i < boundingBox.size(); ++i)
    {
        min = glm::min(min, boundingBox[i]);
    }
    return min;
}

glm::vec3 Character::getMaxBoundingBox()
{
    glm::vec3 max = boundingBox[0];
    for (int i = 1; i < boundingBox.size(); ++i)
    {
        max = glm::max(max, boundingBox[i]);
    }
    return max;
}

void Character::drawBoundingBox()
{
    if (!displayAABB)
        return;
    AABBRenderer->drawWireframeCube(
        size,
        camera->getViewMatrix(),
        camera->getProjectionMatrix());
}

void Character::draw(GLuint programID)
{
    SceneNode::draw(programID);
    targetCubeRenderer->drawWireframeCube(
        glm::vec3(1.f, 1.f, 1.f),
        camera->getViewMatrix(),
        camera->getProjectionMatrix());
}
/**
 * \brief fonction qui gère la gravité du personnage
 * @param deltaTime
 */
void Character::resolveGravity(float &deltaTime)
{
    // Les modes créatif et spectateur ignorent la gravité
    if (gamemode == GAMEMODE_CREATIVE || gamemode == GAMEMODE_SPECTATOR)
    {
        return;
    }

    // Appliquer la gravité selon le contexte (eau ou non)
    if (isInWater)
    {
        // Gravité réduite dans l’eau
        velocity.y += -gravity * 0.3f * deltaTime;

        // Saut dans l’eau (remontée)
        if (keyInput->isKeybindHeld(keybinds->jump))
        {
            velocity.y -= gravity * 0.6f * deltaTime;
        }
        if (keyInput->isKeybindHeld(keybinds->sneak))
        {
            velocity.y += gravity * 0.3f * deltaTime;
        }

        // Limiter la vitesse verticale dans l’eau
        velocity.y = glm::clamp(velocity.y, -4.0f, 2.0f);
    }
    else
    {
        // Gravité normale hors de l’eau
        velocity.y += gravity * deltaTime;
    }

    // Prédire la position suivante
    glm::vec3 nextPosition = getWorldPosition() + velocity * deltaTime;

    // Bounding box mise à jour avec la vitesse verticale
    glm::vec3 minBB = getMinBoundingBox();
    glm::vec3 maxBB = getMaxBoundingBox();

    minBB.y += velocity.y * deltaTime;
    maxBB.y += velocity.y * deltaTime;

    // Réinitialiser l’état
    bool isGrounded = false;
    bool detectedWater = false;

    // Vérifie les collisions dans le volume de la bounding box
    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockId = m_world->getBloc(x, y, z);

                if (blockId == WATER)
                {
                    detectedWater = true;
                }
                else if (blockId != AIR)
                {
                    isGrounded = true;
                    break;
                }
            }
            if (isGrounded)
                break;
        }
        if (isGrounded)
            break;
    }

    // Appliquer la position si non bloqué
    if (!isGrounded)
    {
        translate(glm::vec3(0.f, velocity.y * deltaTime, 0.f));
    }
    else
    {
        velocity.y = 0.0f;
    }

    // Met à jour l’état de l’eau (sera utilisé dans listenAction)
    this->isInWater = detectedWater;
}

void Character::alignWithCamera(const glm::vec3& cameraDirection) {
    if (gamemode == GAMEMODE_SPECTATOR)
        return;
        
    float targetAngle = atan2(cameraDirection.x, cameraDirection.z);
    
    m_transform.m_rotation = DEFAULT_ROTATION;
    rotate(-targetAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    
    updateModelMatrix();
}