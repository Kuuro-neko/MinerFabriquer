// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/imageLoader.h>
#include <TP/Camera/Camera.hpp>
#include <TP/Scene/Scene.hpp>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int terrain_resolution = 32;
float terrain_scale = 1.0;
float terrain_height = 1.0;


//rotation
float angle = 0.;
float zoom = 1.;
/*******************************************************************************/

void create_plan_textured( int n, int m, glm::vec3 scale, std::vector< glm::vec3 > & vertices , std::vector< unsigned short > & indices, std::vector<glm::vec2> &uvs, Texture &heightMap) {
    vertices.clear();
    indices.clear();
    uvs.clear();
    float x, y, z, u, v;
    float ymax = -1000;
    float ymin = 1000;
    for( int i = 0 ; i < n ; ++i ) {
        for( int j = 0 ; j < m ; ++j ) {
            x = float(i)/n-0.5;
            z = float(j)/m-0.5;
            u = float(i)/n;
            v = 1.f - float(j)/m;
            y = heightMap.image.data[(u * heightMap.image.w) + (v * heightMap.image.h) * heightMap.image.w].r/255.f*0.5;
            if (z > ymax) {
                ymax = z;
            }
            if (z < ymin) {
                ymin = z;
            }
            glm::vec3 vertex(
                x * scale.x,
                y * scale.y,
                z * scale.z
            );
            vertices.push_back(vertex);
            uvs.push_back(glm::vec2(u, v));
            
            if (i < n-1 && j < m-1) {
                indices.push_back(i*m+j);
                indices.push_back((i+1)*m+j+1);
                indices.push_back((i+1)*m+j);

                indices.push_back(i*m+j);
                indices.push_back(i*m+j+1);
                indices.push_back((i+1)*m+j+1);
            }
        }
    }
    glUniform1f(glGetUniformLocation(1, "ymin"), ymin);
    glUniform1f(glGetUniformLocation(1, "ymax"), ymax);
}

void create_sphere_textured(int n, int m, MeshObject &mesh) {
    mesh.vertices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    float x, y, z, u, v;
    for( int i = 0 ; i <= n ; ++i ) {
        for( int j = 0 ; j <= m ; ++j ) {
            x = cos(2*M_PI*i/n)*cos(M_PI*j/m - M_PI_2);
            y = sin(2*M_PI*i/n)*cos(M_PI*j/m - M_PI_2);
            z = sin(M_PI*j/m - M_PI_2);
            u = float(i)/n;
            v = 1.f - float(j)/m;
            glm::vec3 vertex(
                x,
                y,
                z
            );
            mesh.vertices.push_back(vertex);
            mesh.uvs.push_back(glm::vec2(u, v));
        }
    }
    for (int i = 0; i < mesh.vertices.size()-n-1; ++i) {
        mesh.triangles.push_back(i);
        mesh.triangles.push_back(i+n);
        mesh.triangles.push_back(i+n+2);

        mesh.triangles.push_back(i);
        mesh.triangles.push_back(i+n+1);
        mesh.triangles.push_back(i+1);
    }
}

SceneNode controllableSphere(
    Transform(
        glm::vec3(0, 5, 0),
        DEFAULT_ROTATION,
        1),
    nullptr);

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
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
    window = glfwCreateWindow( 1024, 768, "TP1 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/

    MeshObject sphereMesh64 = MeshObject();
    create_sphere_textured(64, 64, sphereMesh64);
    sphereMesh64.initializeBuffers();

    MeshObject sphereMesh32 = MeshObject();
    create_sphere_textured(32, 32, sphereMesh32);
    sphereMesh32.initializeBuffers();

    MeshObject sphereMesh16 = MeshObject();
    create_sphere_textured(16, 16, sphereMesh16);
    sphereMesh16.initializeBuffers();

    MeshObject sphereMesh8 = MeshObject();
    create_sphere_textured(8, 8, sphereMesh8);
    sphereMesh8.initializeBuffers();

    MeshObject sphereMesh6 = MeshObject();
    create_sphere_textured(6, 6, sphereMesh6);
    sphereMesh6.initializeBuffers();

    MeshObject terrainMesh = MeshObject();
    Texture heightMap = Texture("../textures/heightmap-rocky.ppm", DONT_INCREMENT_BINDING);
    create_plan_textured(64, 64, glm::vec3(25, 8, 25), terrainMesh.vertices, terrainMesh.triangles, terrainMesh.uvs, heightMap);
    terrainMesh.initializeBuffers();

    Texture terrainTexture = Texture("../textures/cat.png");
    terrainTexture.setSamplerName("PlanetTextureSampler");


    SceneNode root;

    controllableSphere.m_mesh = &sphereMesh64;
    root.addChild(&controllableSphere);

    LODManager lodManager = LODManager(
        camera.getPosition()
    );
    lodManager.addLOD(&sphereMesh64, 12);
    lodManager.addLOD(&sphereMesh32, 18);
    lodManager.addLOD(&sphereMesh16, 25);
    lodManager.addLOD(&sphereMesh8, 32);
    lodManager.addLOD(&sphereMesh6, 40);
    controllableSphere.m_lodManager = &lodManager;

    camera.setTarget(controllableSphere.getWorldPosition());

    Texture controllableSphereTexture = Texture("../textures/s7.ppm");
    controllableSphereTexture.setSamplerName("PlanetTextureSampler");
    controllableSphere.m_texture = &controllableSphereTexture;

    SceneNode terrain(
        Transform(
            glm::vec3(0, 0, 0),
            DEFAULT_ROTATION,
            1),
        &terrainMesh);
    root.addChild(&terrain);
    terrain.m_texture = &terrainTexture;

    SceneNode sunRotationCenter(
        Transform(
            glm::vec3(0, 0, 0),
            DEFAULT_ROTATION,
            1),
        nullptr);
    root.addChild(&sunRotationCenter);

    SceneNode moonRotationCenter(
        Transform(
            glm::vec3(0, 0, 0),
            DEFAULT_ROTATION,
            1),
        nullptr);
    root.addChild(&moonRotationCenter);



    // ---- SUN
    SceneNode sun(
        Transform(
            glm::vec3(0, 800, 0),
            DEFAULT_ROTATION,
            12),
        &sphereMesh64);
    sunRotationCenter.addChild(&sun);
    sun.m_mesh = &sphereMesh64;

    Texture sunTexture = Texture("../textures/s2.ppm");
    sunTexture.setSamplerName("PlanetTextureSampler");
    sun.m_texture = &sunTexture;
    
    // ---- MOON
    SceneNode moon(
        Transform(
            glm::vec3(0, 150, 0),
            DEFAULT_ROTATION,
            5),
        &sphereMesh64);
    moonRotationCenter.addChild(&moon);

    Texture rockTexture = Texture("../textures/s6.ppm");
    rockTexture.setSamplerName("PlanetTextureSampler");
    moon.m_texture = &rockTexture;

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");



    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do{

        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        camera.updateTarget(controllableSphere.getWorldPosition());
        camera.update(deltaTime, window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        GLuint viewMatrixId = glGetUniformLocation(programID, "ViewMatrix");
        glUniformMatrix4fv(viewMatrixId, 1, false, &camera.m_viewMatrix[0][0]);
        GLuint projectionMatrixId = glGetUniformLocation(programID, "ProjectionMatrix");
        glUniformMatrix4fv(projectionMatrixId, 1, false, &camera.m_projectionMatrix[0][0]);


        sunRotationCenter.rotate(currentFrame*0.1, AXIS_X);
        moonRotationCenter.rotate(currentFrame*0.5, glm::vec3(cos(-6.68*M_PI/180), sin(-6.68*M_PI/180), cos(-6.68*M_PI/180)));

        sun.rotate(currentFrame*0.001, AXIS_Y);
        moon.rotate(currentFrame, glm::vec3(cos(-6.68*M_PI/180), sin(-6.68*M_PI/180), cos(-6.68*M_PI/180)));

        
        controllableSphere.keepAboveGround(&terrain);
        root.draw(programID);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    root.cleanupBuffers();
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    glm::vec3 cameraFrontNoUp = camera.getRotation() * VEC_FRONT;
    cameraFrontNoUp.y = 0.f;
    cameraFrontNoUp = normalize(cameraFrontNoUp);

    glm::vec3 cameraRightNoUp = camera.getRotation() * VEC_RIGHT;
    cameraRightNoUp.y = 0.f;
    cameraRightNoUp = normalize(cameraRightNoUp);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        controllableSphere.translate(cameraFrontNoUp * 0.1f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        controllableSphere.translate(cameraFrontNoUp * -0.1f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        controllableSphere.translate(cameraRightNoUp * 0.1f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        controllableSphere.translate(cameraRightNoUp * -0.1f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

