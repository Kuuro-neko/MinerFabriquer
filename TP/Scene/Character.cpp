#include "Character.hpp"

#include <iostream>

using namespace std;

Character::Character(Transform transform, Camera camera, MeshObject *mesh = nullptr, Texture *texture = nullptr)
        : SceneNode(transform, mesh, texture), camera(camera) {
    speed = 1.0f;
}

void Character::move(glm::vec3 direction) {
    this->m_transform.translate(direction * speed);
}

/**
 * \brief fonction qui réalise l'action en fonction de la touche détectée
 * @param key
 */
void Character::listenAction(float dt, GLFWwindow *window) {

    glm::vec3 cameraFrontNoUp = camera.getRotation() * VEC_FRONT;
    cameraFrontNoUp.y = 0.f;
    cameraFrontNoUp = normalize(cameraFrontNoUp);
    glm::vec3 cameraRightNoUp = camera.getRotation() * VEC_RIGHT;
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

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//        std::cout << "inventaire" << std::endl;

    //si on a un

}