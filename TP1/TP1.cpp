#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/shader.hpp>
#include <TP/Camera/Camera.hpp>
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include <TP/Scene/Skybox.hpp>
#include <TP/Scene/Character.hpp>

GLFWwindow *window;

using namespace std;
using namespace glm;


void processInput(GLFWwindow *window, float dt);


Camera camera;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

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
                glm::vec3(0, 5, 0),
                DEFAULT_ROTATION,
                1),
        &camera,
        nullptr,
        nullptr
);

int main(void) {
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    camera.init();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "TP1 - GLFW", NULL, NULL);
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
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

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

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/

    SceneNode root;

    // Our first chunk :D
    MeshObject chunkMesh = MeshObject();
    VoxelChunk chunk = VoxelChunk(16, 16, 16, Transform(
            glm::vec3(0, 0, 0),
            DEFAULT_ROTATION,
            1.f), &chunkMesh);
    chunk.setBloc(2, 3, 2, LOG_OAK);
    chunk.setBloc(2, 4, 2, LOG_OAK);
    chunk.setBloc(2, 5, 2, LOG_OAK);
    chunk.setBloc(2, 6, 2, LEAVES_OAK);
    chunk.setBloc(2, 5, 3, LEAVES_OAK);
    chunk.setBloc(3, 5, 2, LEAVES_OAK);
    chunk.setBloc(2, 5, 1, LEAVES_OAK);
    chunk.setBloc(1, 5, 2, LEAVES_OAK);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            chunk.setBloc(i, 0, j, STONE);
            chunk.setBloc(i, 1, j, DIRT);
            chunk.setBloc(i, 2, j, GRASS);
        }
    }
    chunk.generateMesh();
    root.addChild(&chunk);


    MeshObject characterMesh = MeshObject();
    create_sphere_textured(64, 64, characterMesh);
    characterMesh.initializeBuffers();
    character.m_mesh = &characterMesh;
    root.addChild(&character);
    camera.setTarget(character.getWorldPosition());
    character.m_texture = TextureAtlas::getInstance().getTexture();

    Skybox skybox = Skybox();
    skybox.generateBuffers();
    root.addChild(&skybox);


    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do {
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window, deltaTime);
        character.listenAction(deltaTime, window);
        camera.updateTarget(character.getWorldPosition());
        camera.update(deltaTime, window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        GLuint viewMatrixId = glGetUniformLocation(programID, "ViewMatrix");
        glUniformMatrix4fv(viewMatrixId, 1, false, &camera.m_viewMatrix[0][0]);
        GLuint projectionMatrixId = glGetUniformLocation(programID, "ProjectionMatrix");
        glUniformMatrix4fv(projectionMatrixId, 1, false, &camera.m_projectionMatrix[0][0]);

        root.draw(programID);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    root.cleanupBuffers();
    glDeleteProgram(programID);

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
