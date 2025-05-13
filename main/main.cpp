#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/shader.hpp>
#include <TP/Camera/Camera.hpp>
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include "TP/Character/Character.hpp"
#include "TP/Menu/Menu.hpp"
#include <TP/GUI/Crosshair.hpp>
#include <TP/Entities/Entity.hpp>
#include <TP/Scene/Clouds.hpp>
#include <TP/GUI/HUD.hpp>
#include <TP/FileSystem/SaveManager.hpp>
#include <Defines.hpp>
#include <TP/Scene/WorldGenerator.hpp>
#include "../TP/Entities/HumanoidEntity.hpp"
#include <TP/Textures/TextureManager.hpp>

GLFWwindow *window;

using namespace std;
using namespace glm;

// Defaut
int windowWidth = 1280;
int windowHeight = 720;

HUD *hud = nullptr;

Camera camera;
// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
float FPS = 0.0f;

// rotation
float angle = 0.;
float zoom = 1.;

int displayNormals = 0;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);



Character* character = new Character(
        Transform(
                glm::vec3(0, GROUND_LEVEL+8, 0),
                DEFAULT_ROTATION,
                1),
        &camera
);


void UpdateFPS() {
    static double lastTime = glfwGetTime();
    static unsigned int counter = 0;
    counter++;
    double currentTime = glfwGetTime();
    if ((currentTime - lastTime) >= 1.0) { // 1 second has passed
        FPS = counter;
        counter = 0;
        char FPSstr[128];
        snprintf(FPSstr, sizeof(FPSstr), "FPS: %.2f", FPS);
        glfwSetWindowTitle(window, FPSstr);
        lastTime = currentTime;
    }
}

int main(void) {
    Menu menu;
    SaveManager &saveManager = SaveManager::getInstance();

    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    camera.init();
    Frustrum frustum(camera, DEFAULT_RATIO, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
    frustum.update();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, "Miner-Fabriquer", NULL, NULL);
    KeyInput characterInputManager = KeyInput(Keybinds::getInstance().getKeysToMonitorForCharacter());
    KeyInput menuInputManager = KeyInput(Keybinds::getInstance().getKeysToMonitorForMenu());
    menuInputManager.setIsEnabled(false);
    KeyInput::setupKeyInputs(*window);
    character->setKeyInput(&characterInputManager);
    camera.setKeyInput(&characterInputManager);


    if (window == NULL) {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glEnable(GL_MULTISAMPLE);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSwapInterval(0);
    
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);

    // Dark blue background
    glClearColor(0.4f, 0.6f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("../shader/vertex_shader.glsl", "../shader/fragment_shader.glsl");
    GLuint wireframeProgramID = LoadShaders("../shader/vertex_shader_wireframe.glsl",
                                            "../shader/fragment_shader_wireframe.glsl");
    Renderer renderer = Renderer(wireframeProgramID);
    Renderer rendererCharacterBoundingBox = Renderer(wireframeProgramID);
    rendererCharacterBoundingBox.setHighlight(character->getMinBoundingBox());
    GLuint cubemapProgramID = LoadShaders("../shader/cubemap_vertex_shader.glsl",
                                          "../shader/cubemap_fragment_shader.glsl");
    GLuint cloudsProgramID = LoadShaders("../shader/clouds_vertex_shader.glsl",
                                         "../shader/clouds_fragment_shader.glsl");

    hud = new HUD(windowWidth, windowHeight);
    character->setHUD(hud);

    Texture lightMap = Texture("../textures/lightmap.png");
    lightMap.setSamplerName("LightmapSampler");
    lightMap.genTexture(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
    lightMap.bind(programID);

    CubemapTexture cubemapTexture = CubemapTexture(cubemapProgramID);


    SceneNode root;
    World *world = new World();
    root.addChild(world);
    root.addChild(character); 
    world->setCamera(camera);
    world->setDoDaylightCycle(false);

   

    //pass the world to the save manager
    saveManager.setWorld(world);
    saveManager.setCharacter(character);

    if (!saveManager.isDataFolderContainsOtherFolder()) {
        std::cout << "No world folder found. Generating a new world->.." << std::endl;

        std::string saveFolder = Menu::createWorld();
        std::string seedStr = menu.askSeed();
        WorldGenerator::getInstance().setSeed(seedStr);
        saveManager.setSaveFolderPath(saveFolder);
        world->initialGeneration();
        saveManager.saveWorldFile(); // Save the world data after generation
        saveManager.createPlayerDataFile();
        saveManager.createSeedFile();
    } else {
        int choice = Menu::chooseLoadOrNewWorld();
        switch (choice) {
            case MENU_CREATE: {
                std::string saveFolder = Menu::createWorld();
                std::string seedStr = menu.askSeed();
                WorldGenerator::getInstance().setSeed(seedStr);
                std::cout << "Creating world in: " << saveFolder << std::endl;
                saveManager.setSaveFolderPath(saveFolder);
                world->initialGeneration();
                saveManager.saveWorldFile(); // Save the world data after generation
                saveManager.createPlayerDataFile();
                saveManager.createSeedFile();
                break;
            }
           case MENU_LOAD: {
               std::string worldPath = menu.chooseWorld();
               std::cout << "Loading world from: " << worldPath << std::endl;
               saveManager.setSaveFolderPath(worldPath);
               std::vector<SaveManager::ChunkColumnEntry> colEntries = saveManager.loadWorldFile();
               for (const auto &colEntry : colEntries) {
                    std::shared_ptr<ChunkColumn> column = std::make_shared<ChunkColumn>(colEntry.worldX, colEntry.worldZ);
                    for (int i = 0; i < CHUNK_SIZE; ++i) {
                        for (int j = 0; j < CHUNK_SIZE; ++j) {
                            column->setHeightmapValue(i, j, colEntry.heightmap[i][j]);
                        }
                    }
                    for (int y = GENERATION_SIZE_Y - 1; y >= 0; --y) {
                        auto newChunk = std::make_shared<VoxelChunk>();
                        newChunk->translate(glm::vec3(colEntry.worldX * CHUNK_SIZE, y * CHUNK_SIZE, colEntry.worldZ * CHUNK_SIZE));
                        newChunk->m_chunkCoords = glm::ivec3(colEntry.worldX, y, colEntry.worldZ);
                        column->addChunk(newChunk);

                        auto chunkEntry = colEntry.chunks[y];
                        
                        if (newChunk) {
                            for (int bx = 0; bx < CHUNK_SIZE; ++bx) {
                                for (int by = 0; by < CHUNK_SIZE; ++by) {
                                    for (int bz = 0; bz < CHUNK_SIZE; ++bz) {
                                        newChunk->generationSetBloc(bx, by, bz, (int) chunkEntry.blocksID[bx * CHUNK_SIZE * CHUNK_SIZE + by * CHUNK_SIZE + bz]);
                                        // cast from int8 to int
                                        newChunk->setLightLevel(bx, by, bz, (int) chunkEntry.lightmap[bx * CHUNK_SIZE * CHUNK_SIZE + by * CHUNK_SIZE + bz]);
                                    }
                                }
                            }
                        }
                    }
                    world->addColumn(column);
                    column->assignWorld(world);
                    for (auto &chunk : column->getChunks()) {
                        world->emplaceChunk(chunk);
                        chunk->dirty = true;
                    }
               }
               saveManager.readSeedFile();
               break;
           }
            default:
                std::cerr << "Invalid choice. Exiting..." << std::endl;
                glfwTerminate();
                return -1;

        }

    }
    saveManager.loadPlayerData();
    std::cout << "World loaded from: " << saveManager.getSaveFolderPath() << std::endl;
    saveManager.startAutoSave();
    saveManager.saveSeedFile();
    world->startWorkerThread();

    // Associer le monde au personnage
    character->m_world = world;

    // Ajouter le personnage au monde
    HumanoidEntity *characterModel = new HumanoidEntity();

    // The rest of your code can remain the same:
    characterModel->setFPSActive(&camera.m_attached);
    characterModel->generateHumanoidMesh(-0.38f);

    characterModel->setTexture(TextureManager::getInstance().getPBRTexture("steve"));
    character->addChild(characterModel);
    character->setCharacterModel(characterModel);
    
    character->setWireframeRenderers(wireframeProgramID);
    camera.setTarget(character->getWorldPosition());

    Texture cloudTex = Texture("../textures/clouds.png");
    Clouds clouds = Clouds(cloudTex, 0.0005f, cloudsProgramID);

    world->spawnEntities();


/*     Entity* Mr_Vincell = new Entity();
    Mr_Vincell->generateHumanoidMesh(0.0f);
    Texture* playerTexture2 = new Texture("../textures/Mr_Vincell.png");
    Mr_Vincell->setTexture(playerTexture2);
    Mr_Vincell->translate(glm::vec3(8, 11, 10));
    Mr_Vincell->rotate(glm::radians(180.0f), glm::vec3(0, 1, 0));
    root.addChild(Mr_Vincell);

    Entity* Kuurpo = new Entity();
    Kuurpo->generateHumanoidMesh(0.0f);
    Texture* playerTexture3 = new Texture("../textures/Kuurpo.png");
    Kuurpo->setTexture(playerTexture3);
    Kuurpo->translate(glm::vec3(9, 11, 10));
    Kuurpo->rotate(glm::radians(180.0f), glm::vec3(0, 1, 0));
    root.addChild(Kuurpo);

    Entity* Akkuun = new Entity();
    Akkuun->generateHumanoidMesh(0.0f);
    Texture* playerTexture4 = new Texture("../textures/Akkuun.png");
    Akkuun->setTexture(playerTexture4);
    Akkuun->translate(glm::vec3(7, 11, 10));
    Akkuun->rotate(glm::radians(180.0f), glm::vec3(0, 1, 0));
    root.addChild(Akkuun); */





    glfwSetScrollCallback(window, [](GLFWwindow *window, double xOffset, double yOffset) {
        character->scrollCallback(window, xOffset, yOffset);
    });

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    do {
        UpdateFPS();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        world->update(deltaTime);

        
        // Poll inputs
        glfwPollEvents();
        
        auto time_a = std::chrono::high_resolution_clock::now();

        // on change listen action, on met à jour un vecteur de direction qui est !=1 quand un touche est tapé sinon 0
        character->listenAction(deltaTime);
        camera.updateTarget(character->getWorldPosition());
        camera.update(deltaTime, window);


        world->updateLoadedChunks();
        frustum.update();
        world->updateVisibleChunk(frustum);


        world->resolveCollisions(*character, world);
        character->resolveGravity(deltaTime);

        
        world->renderEntities( wireframeProgramID);
        world->resolveEntityGravity(deltaTime);
        world->resolveEntityCollisions(deltaTime);
        world->updateEntities(deltaTime);
       

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        cubemapTexture.draw(camera);

        // Use our shader
        glUseProgram(programID);

        GLuint viewMatrixId = glGetUniformLocation(programID, "ViewMatrix");
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
        GLuint projectionMatrixId = glGetUniformLocation(programID, "ProjectionMatrix");
        glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);
        GLuint displayNormalId = glGetUniformLocation(programID, "displayNormals");
        glUniform1i(displayNormalId, displayNormals);
        GLuint camPos = glGetUniformLocation(programID, "camPos");
        glUniform3f(camPos, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        GLuint time = glGetUniformLocation(programID, "time");
        glUniform1f(time, world->getTime());

        lightMap.bind(programID);

        glUseProgram(cloudsProgramID);

        GLuint viewMatrixIdC = glGetUniformLocation(cloudsProgramID, "ViewMatrix");
        glUniformMatrix4fv(viewMatrixIdC, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
        GLuint projectionMatrixIdC = glGetUniformLocation(cloudsProgramID, "ProjectionMatrix");
        glUniformMatrix4fv(projectionMatrixIdC, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);

        glUseProgram(programID);



        root.draw(programID);

        // Restore shader program and matrices for the scene
        glUseProgram(programID);
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);

        if (characterInputManager.isKeybindPressed(Keybinds::getInstance().toggleChunkBorders)) {
            displayNormals = displayNormals == 0 ? 1 : 0;
        }
        if (characterInputManager.isKeybindPressed(Keybinds::getInstance().toggleWireframe)) {
            world->wireframe = !world->wireframe;
        }
        if (characterInputManager.isKeybindPressed({Keybinds::getInstance().getToggleDebug()})) {
            std::cout << "Seed string : " << WorldGenerator::getInstance().getSeedStr() << std::endl;
        }
        if (characterInputManager.isKeybindPressed({Keybinds::getInstance().spawnEntities})) {
            world->spawnEntities();
        }
        if (characterInputManager.isKeybindPressed({Keybinds::getInstance().fireTNT})) {
            world->spawnTNT(camera.getPosition(), camera.getFront(), programID);
        }

        character->drawBoundingBox();



        if (character->isHUDVisible())
            hud->render();
        if (character->isHUDVisible()) hud->render();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        clouds.draw(currentFrame, *character);

        // Swap buffers
        glfwSwapBuffers(window);

        // Update the input managers
        characterInputManager.update();
        menuInputManager.update();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    saveManager.saveWorldFile();
    saveManager.saveCharacterFile();
    root.cleanupBuffers();
    cubemapTexture.cleanupBuffers();

    // delete &hud;

    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    camera.m_projectionMatrix = glm::perspective(glm::radians(camera.getFOV()), (float) width / (float) height,
                                                 camera.getNearPlane(), camera.getFarPlane());
    hud->updateWindowSize(width, height);
}

