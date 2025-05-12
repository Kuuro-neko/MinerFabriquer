#include "Entity.hpp"

Entity::Entity()
{
    // Default initialization
}

Entity::~Entity()
{
    // Base destructor
}

void Entity::draw(GLuint programID)
{
    if (isFPSActive())
    {
        SceneNode::draw(programID);
    }
}

void Entity::cleanupBuffers()
{
    // Call parent class implementation
    SceneNode::cleanupBuffers();
}

void Entity::setTexture(Texture *texture)
{
    m_texture = texture;
}

void Entity::setFPSActive(bool *attached)
{
    FPSActive = attached;
}

bool Entity::isFPSActive()
{
    if (FPSActive && *FPSActive)
    {
        return true;
    }
    else
    {
        return false;
    }
}