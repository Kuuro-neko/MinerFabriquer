#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utils/Transform.hpp"
#include "TP/Scene/MeshObject.hpp"
#include "TP/Scene/Texture.hpp"
#include "TP/Scene/SceneNode.hpp"
#include "TP/Camera/Camera.hpp"
#include "TP/Scene/VoxelChunk.hpp"
#include "Inventory.hpp"
#include "TP/Scene/Renderer.hpp"
#include <TP/Scene/World.hpp>
#include <TP/Input/KeyInput.hpp>
#include <TP/Input/KeyBinds.hpp>
#include "vector"
#include <TP/GUI/HUD.hpp>
#include <Defines.hpp>
#include "TP/Scene/Entity.hpp"


class Character : public SceneNode {

public:
    Character(Transform transform, Camera *camera, World *world = nullptr, MeshObject *mesh = nullptr,
              Texture *texture = nullptr);

    inline void setWireframeRenderers(GLuint wireframeProgramID) {
        this->targetCubeRenderer = new Renderer(wireframeProgramID);
        this->AABBRenderer = new Renderer(wireframeProgramID);
    }
    inline void applyGravity() {translate(glm::vec3(0.f, -gravity, 0.f));}
    inline glm::vec3 getSize() { return size; }
    inline unsigned char getGamemode() { return gamemode; }
    inline void setKeyInput(KeyInput *keyInput) { this->keyInput = keyInput; }
    void draw(GLuint programID) override;


    void listenAction(float dt);
    void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
    void update(float dt);
    void updateBoundingBox();
    void move(glm::vec3 direction);
    void drawBoundingBox();


    glm::vec3 getMinBoundingBox();
    glm::vec3 getMaxBoundingBox();

    World *m_world;
    Camera *camera;
    Inventory *inventory;
    glm::vec3 velocity = glm::vec3(0.f);
    glm::vec3 vecteurDirection = glm::vec3(0.f, 0.f, 0.f);
    bool sneaking = false;
    bool sprinting = false;

    void resolveGravity(float &deltaTime);
    bool isInWater = false;

    void setHUD(HUD* hud) { m_hud = hud; }

    void setCharacterModel(Entity* model);

private:

    void updateClosestBlock(BlocDatabase &database);
    void breakBlock(BlocDatabase &database);
    void putBlock(BlocDatabase &database);
    void setSelectedBlock(BlocDatabase &database);
    void updateCamera();

    inline void resetBreakCooldown() {
        breakCooldown = 0.f;
    }
    inline void resetPlaceCooldown() {
        placeCooldown = 0.f;
    }

    Renderer *targetCubeRenderer = nullptr;
    Renderer *AABBRenderer = nullptr;
    KeyInput *keyInput;
    Keybinds *keybinds = &Keybinds::getInstance();

    float speed = DEFAULT_SPEED;
    float sneakSpeed = DEFAULT_SNEAK_SPEED;
    float sprintSpeed = DEFAULT_SPRINT_SPEED;
    float maxInteractionDistance = 6.f;
    float breakCooldown = std::numeric_limits<float>::max();
    float placeCooldown = std::numeric_limits<float>::max();

    float gravity = -9.81f;

    std::vector<glm::vec3> boundingBox;
    glm::vec3 size;
    glm::vec3 blocPlusProche;

    int facePlusProche = -1;
    bool intersection = false;

    // To not toggle debug if another debug keybind involving toggleDebug Key was inputted
    bool shouldToggleDebug = true;
    bool displayAABB = false;
    unsigned char gamemode = GAMEMODE_SURVIVAL;
    unsigned char prevGamemode = GAMEMODE_SURVIVAL;

    HUD* m_hud = nullptr;

    void alignWithCamera(const glm::vec3& cameraDirection);
    void initializePlayerAnimations(Entity* characterModel);
    void createWalkingPoses(Entity* characterModel);
    Entity* m_characterModel = nullptr;
};

#endif // CHARACTER_HPP