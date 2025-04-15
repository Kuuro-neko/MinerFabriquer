#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/shader.hpp>
#include <TP/Camera/Camera.hpp>
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include "TP/Character/Character.hpp"
#include "TP/Camera/Frustrum.hpp"
#include <TP/GUI/Crosshair.hpp>

#define CHUNK_SIZE 16

GLFWwindow *window;

using namespace std;
using namespace glm;

int windowWidth = 1024;
int windowHeight = 768;


void processInput(GLFWwindow *window, float dt);


Camera camera;
// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;
float FPS = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;


void create_sphere_textured(int n, int m, MeshObject &mesh) {
    mesh.vertices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    float x, y, z, u, v;
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            x = cos(2 * M_PI * i / n) * cos(M_PI * j / m - M_PI_2);
            y = sin(2 * M_PI * i / n) * cos(M_PI * j / m - M_PI_2);
            z = sin(M_PI * j / m - M_PI_2);
            u = float(i) / n;
            v = 1.f - float(j) / m;
            glm::vec3 vertex(
                    x,
                    y,
                    z
            );
            mesh.vertices.push_back(vertex);
            mesh.uvs.push_back(glm::vec2(u, v));
        }
    }
    for (int i = 0; i < mesh.vertices.size() - n - 1; ++i) {
        mesh.triangles.push_back(i);
        mesh.triangles.push_back(i + n);
        mesh.triangles.push_back(i + n + 2);

        mesh.triangles.push_back(i);
        mesh.triangles.push_back(i + n + 1);
        mesh.triangles.push_back(i + 1);
    }
}


Character character = Character(
        Transform(
                glm::vec3(0, 0, 0),
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
    // Initialise GLFW
    if (!glfwInit()) {
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
    window = glfwCreateWindow(windowWidth, windowHeight, "main - GLFW", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

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
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    GLuint programID2 = LoadShaders("vertex_shader_wireframe.glsl", "fragment_shader_wireframe.glsl");
    Renderer renderer = Renderer(programID2);
    GLuint crosshairProgramID = LoadShaders("vertex_shader_2D.glsl", "fragment_shader_crosshair.glsl");

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(programID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
    }

    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/

    Crosshair crosshair = Crosshair(crosshairProgramID, 0.03f);

    SceneNode root;

    World world = World();
    root.addChild(&world);
    world.setCamera(camera);

    character.m_world = &world;

    MeshObject characterMesh = MeshObject();
    create_sphere_textured(64, 64, characterMesh);
    characterMesh.initializeBuffers();
    character.m_mesh = &characterMesh;
    root.addChild(&character);
    character.setRenderer(&renderer);
    camera.setTarget(character.getWorldPosition());
    character.m_texture = TextureAtlas::getInstance().getTexture();

    //tentative de créé une tête 
    MeshObject headMesh = MeshObject();
    

    glfwSetScrollCallback(window, [](GLFWwindow *window, double xOffset, double yOffset) {
        character.scrollCallback(window, xOffset, yOffset);
    });


    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    do {
        // Measure speed
        // per-frame time logic
        // --------------------
        UpdateFPS();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window, deltaTime);
        character.listenAction(deltaTime, window, BlocDatabase::getInstance());
        camera.updateTarget(character.getWorldPosition());
        camera.update(deltaTime, window);

        frustum.update();
        //TEST sur le chhunk 3
        //On met dans World la liste des chunks présent dans le furstrum dans l'attribute visblechunks, puis on ne dessiner qu'eux
        world.updateVisibleChunk(frustum);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        GLuint viewMatrixId = glGetUniformLocation(programID, "ViewMatrix");
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
        GLuint projectionMatrixId = glGetUniformLocation(programID, "ProjectionMatrix");
        glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);


        root.draw(programID);
        renderer.drawWireframeCube(
                glm::vec3(1.f, 1.f, 1.f),
                camera.getViewMatrix(),
                camera.getProjectionMatrix()
        );

        crosshair.render();
        // Restore shader program and matrices for the scene
        glUseProgram(programID);
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);


        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    root.cleanupBuffers();
    crosshair.cleanupBuffers();

    glDeleteProgram(programID);
    // glDeleteProgram(crosshairProgramID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float dt) {


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

