#pragma once

#include <TP/Scene/Texture.hpp>
#include <TP/Scene/MeshObject.hpp>  
#include <utils/Transform.hpp>
#include <TP/Character/Character.hpp>

class Clouds
{
private:
    float scrollSpeed;
    float height = 100.0f;
    Texture *texture;
    GLuint programId;
    MeshObject mesh;
    Transform transform;
    glm::mat4 ModelMatrix;
public:
    Clouds(Texture &cloudTex, float speed, GLuint programID);

    void draw(float time, Character &character);
};