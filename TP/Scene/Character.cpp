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

//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
//        std::cout << "inventaire" << std::endl;
//    }


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        //on fait un coup de pioche
        breakBlock();
    }

}
/**
 * \brief fonction qui réalise l'action de casser un bloc
 */
void Character::breakBlock() {
    //Etape 1 : on envoie un rayon depuis le front de la caméra
    glm::vec3 rayOrigin = camera.getPosition();
    glm::vec3 rayDirection = camera.getRotation() * VEC_FRONT;
    rayDirection = normalize(rayDirection); //vecteur normalisé
    glm::vec3 rayEnd = rayOrigin + rayDirection * 10.f; //10m de portée
    //Etape 2 : on teste si le rayon touche un bloc
    if(rayDirection.x > 0.5f || rayDirection.y > 0.5f || rayDirection.z > 0.5f) {
        std::cout << "rayon touche un bloc" << std::endl;
    } else {
        std::cout << "rayon ne touche pas de bloc" << std::endl;
    }

    //Etape 3 : si oui, on récupère son type

    //Etape 4 : on le détruit

}