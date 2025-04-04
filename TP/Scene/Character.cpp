#include "Character.hpp"

#include <iostream>

using namespace std;

Character::Character(Transform transform, Camera *camera, MeshObject *mesh = nullptr, Texture *texture = nullptr)
        : SceneNode(transform, mesh, texture), camera(camera) {
    speed = 2.5;
    camera->setPosition(transform.m_translation + CAMERA_POSITION_RELATIVE_TO_PLAYER);
}

void Character::move(glm::vec3 direction) {
    translate(direction * speed);
    camera->setPosition(getWorldPosition() + CAMERA_POSITION_RELATIVE_TO_PLAYER);
}

/**
 * \brief fonction qui réalise l'action en fonction de la touche détectée
 * @param key
 */
void Character::listenAction(float dt, GLFWwindow *window, VoxelChunk &chunkActuel, BlocDatabase &database) {

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


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        //on fait un coup de pioche
        breakBlock(chunkActuel, database);
    }

}

/**
 * \brief fonction qui réalise l'action de casser un bloc
 */
void Character::breakBlock(VoxelChunk &chunkActuel, BlocDatabase &database) const {
    glm::vec3 directionNormalized = normalize(camera->getRotation() * VEC_FRONT);
    Ray rayon(camera->getPosition(), directionNormalized);
    glm::vec3 rayDirection = normalize(rayon.direction);

    //liste des blocs intersectés
    std::vector<glm::vec3> blocsIntersectes;

    //ETAPE 1 : on parcourt tous les blocs du chunk actuel
    for (int x = 0; x < chunkActuel.m_sizeX; x++) {
        for (int y = 0; y < chunkActuel.m_sizeY; y++) {
            for (int z = 0; z < chunkActuel.m_sizeZ; z++) {
                //on récupère l'id du bloc via la database
                int idBloc = chunkActuel.m_cubes[x][y][z];

                //on récupère la position du bloc -> chunckTransform + position du bloc
                glm::vec3 blocPosition = chunkActuel.getWorldPosition() + glm::vec3(x, y, z);
                if (database.isUnbreakable(idBloc)) continue; // si c'est de l'air on skip


                //on vérifie si le rayon intersecte le bloc
                if (rayon.rayIntersectsAABB(rayon, blocPosition, blocPosition + glm::vec3(1.f
                ), maxInteractionDistance)) {
                    blocsIntersectes.push_back(blocPosition); // on ajoute le bloc dans la liste des éléments intersecté

                }
            }
        }
    }
    //ETAPE 2 : on casse le bloc le plus proche

    // on parcourt la liste des blocs intersectés par le rayon
    if (blocsIntersectes.size() > 0) {
        //on récupère le bloc le plus proche
        glm::vec3 blocPlusProche = blocsIntersectes[0];
        float distanceMin = glm::distance(camera->getPosition(), blocPlusProche);
        for (int i = 1; i < blocsIntersectes.size(); i++) {
            float distance = glm::distance(camera->getPosition(), blocsIntersectes[i]);
            if (distance < distanceMin) {
                distanceMin = distance;
                blocPlusProche = blocsIntersectes[i];
            }
        }
        //on casse le bloc le plus proche -> on remplace le bloc par de l'air
        // on affiche le type de bloc cassé
        int idBlocCasse = chunkActuel.removeBlock(blocPlusProche.x, blocPlusProche.y, blocPlusProche.z);
        std::cout << "Bloc cassé : " << database.getBloc(idBlocCasse)->name << std::endl;

    }
}
