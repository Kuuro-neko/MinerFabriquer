#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/shader.hpp>
#include <TP/Camera/Camera.hpp>
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include "TP/Character/Character.hpp"
#include <TP/GUI/Crosshair.hpp>
#include <TP/Scene/Entity.hpp>
#include <TP/GUI/HUD.hpp>
#include <TP/FileSystem/SaveManager.hpp>
#include <Defines.hpp>

#define CHUNK_SIZE 16

GLFWwindow *window;

using namespace std;
using namespace glm;

// Defaut
int windowWidth = 1280;
int windowHeight = 720;

// int windowWidth = 2560;
// int windowHeight = 1440;

Camera camera;
// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
float FPS = 0.0f;

// rotation
float angle = 0.;
float zoom = 1.;

int displayNormals = 0;

void create_cube_textured(glm::vec3 size, MeshObject &mesh)
{
    mesh.vertices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();

    glm::vec3 p[] = {
        {-size.x, -size.y, -size.z},
        {size.x, -size.y, -size.z},
        {size.x, size.y, -size.z},
        {-size.x, size.y, -size.z},
        {-size.x, -size.y, size.z},
        {size.x, -size.y, size.z},
        {size.x, size.y, size.z},
        {-size.x, size.y, size.z}};

    // Définir les faces du cube avec 4 sommets par face
    int face_indices[6][4] = {
        {0, 1, 2, 3}, // back
        {5, 4, 7, 6}, // front
        {4, 0, 3, 7}, // left
        {1, 5, 6, 2}, // right
        {3, 2, 6, 7}, // top
        {4, 5, 1, 0}  // bottom
    };

    for (int i = 0; i < 6; ++i)
    {
        // 4 sommets pour chaque face
        mesh.vertices.push_back(p[face_indices[i][0]]);
        mesh.vertices.push_back(p[face_indices[i][1]]);
        mesh.vertices.push_back(p[face_indices[i][2]]);
        mesh.vertices.push_back(p[face_indices[i][3]]);

        // UVs correspondants (même pour chaque face)
        mesh.uvs.push_back({0.0f, 0.0f});
        mesh.uvs.push_back({1.0f, 0.0f});
        mesh.uvs.push_back({1.0f, 1.0f});
        mesh.uvs.push_back({0.0f, 1.0f});

        // 2 triangles pour former la face
        int start = i * 4;
        mesh.triangles.push_back(start);
        mesh.triangles.push_back(start + 1);
        mesh.triangles.push_back(start + 2);

        mesh.triangles.push_back(start);
        mesh.triangles.push_back(start + 2);
        mesh.triangles.push_back(start + 3);
    }

    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, 0, -1)); // back
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, 0, 1)); // front
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(-1, 0, 0)); // left
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(1, 0, 0)); // right
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, 1, 0)); // top
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, -1, 0)); // bottom
}

Character character = Character(
    Transform(
        glm::vec3(14, 61, 14),
        DEFAULT_ROTATION,
        1),
    &camera);

void UpdateFPS()
{
    static double lastTime = glfwGetTime();
    static unsigned int counter = 0;
    counter++;
    double currentTime = glfwGetTime();
    if ((currentTime - lastTime) >= 1.0)
    { // 1 second has passed
        FPS = counter;
        counter = 0;
        char FPSstr[128];
        snprintf(FPSstr, sizeof(FPSstr), "FPS: %.2f", FPS);
        glfwSetWindowTitle(window, FPSstr);
        lastTime = currentTime;
    }
}

int main(void)
{
    SaveManager &saveManager = SaveManager::getInstance();
    saveManager.loadPlayerData(character);
    saveManager.startAutoSave(character);

    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    camera.init();
    Frustrum frustum(camera, 4.0f / 3.0f, 0.1f, 100.f);
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
    character.setKeyInput(&characterInputManager);
    camera.setKeyInput(&characterInputManager);

    if (window == NULL)
    {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glEnable(GL_MULTISAMPLE);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
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
    // glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    GLuint wireframeProgramID = LoadShaders("vertex_shader_wireframe.glsl", "fragment_shader_wireframe.glsl");
    Renderer renderer = Renderer(wireframeProgramID);
    Renderer rendererCharacterBoundingBox = Renderer(wireframeProgramID);
    rendererCharacterBoundingBox.setHighlight(character.getMinBoundingBox());
    GLuint cubemapProgramID = LoadShaders("cubemap_vertex_shader.glsl", "cubemap_fragment_shader.glsl");

    HUD hud = HUD(windowWidth, windowHeight);
    character.setHUD(&hud);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(programID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
    }

    Texture lightMap = Texture("../textures/lightmap.png");
    lightMap.setSamplerName("LightmapSampler");
    lightMap.genTexture(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
    lightMap.bind(programID);

    CubemapTexture cubemapTexture = CubemapTexture(cubemapProgramID);

    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/

    SceneNode root;
    World world = World();
    root.addChild(&world);
    world.setCamera(camera);
    world.setDoDaylightCycle(false);

    // Passer l'instance du monde au SaveManager
    saveManager.setWorld(&world);

    if (!saveManager.isWorldFileEmpty())
    {
        world.generation();
        // Si le fichier n'existe pas, générer un nouveau monde
        std::cout << "No world file found. Generating a new world..." << std::endl;
        saveManager.saveWorldFile(); // Sauvegarder le monde après la génération
    }
    else
    {
        // Si le fichier existe, charger le monde
        std::cout << "Loading world data file..." << std::endl;
        saveManager.loadWorldFile();
    }

    // Associer le monde au personnage
    character.m_world = &world;

    // Ajouter le personnage au monde
    Entity *characterModel = new Entity();
    characterModel->setFPSActive(&camera.m_attached);
    characterModel->generateHumanoidMesh(-0.38f); // Position à 0 car il sera enfant du Character
    Texture *playerTexture = new Texture("../textures/steve.png");
    characterModel->setTexture(playerTexture);
    character.addChild(characterModel);
    root.addChild(&character);
    character.setWireframeRenderers(wireframeProgramID);
    camera.setTarget(character.getWorldPosition());


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

    glfwSetScrollCallback(window, [](GLFWwindow *window, double xOffset, double yOffset)
                          { character.scrollCallback(window, xOffset, yOffset); });

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    do
    {
        UpdateFPS();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        world.update(deltaTime);

        // Poll inputs
        glfwPollEvents();

        // on change listen action, on met à jour un vecteur de direction qui est !=1 quand un touche est tapé sinon 0
        character.listenAction(deltaTime);
        camera.updateTarget(character.getWorldPosition());
        camera.update(deltaTime, window);

        frustum.update();
        world.updateVisibleChunk(frustum);

        world.resolveCollisions(character, &world);
        character.resolveGravity(deltaTime);
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
        glUniform1f(time, world.getTime());

        lightMap.bind(programID);

        root.draw(programID);

        // Restore shader program and matrices for the scene
        glUseProgram(programID);
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);

        if (characterInputManager.isKeybindPressed(Keybinds::getInstance().toggleChunkBorders))
        {
            displayNormals = displayNormals == 0 ? 1 : 0;
        }

        character.drawBoundingBox();

        if (character.isHUDVisible())
            hud.render();

        // Swap buffers
        glfwSwapBuffers(window);

        // Update the input managers
        characterInputManager.update();
        menuInputManager.update();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
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
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    // marche pas : camera.m_projectionMatrix = glm::perspective(glm::radians(camera.getFOV()), (float) width / (float) height, camera.getNearPlane(), camera.getFarPlane());
}
