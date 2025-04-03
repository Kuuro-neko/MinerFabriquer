#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout(location = 1) in vec2 vertexUV;

out vec2 UV;

void main(){

        // TODO : Output position of the vertex, in clip space : MVP * position
        gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertices_position_modelspace,1);
        UV = vertexUV;
}

