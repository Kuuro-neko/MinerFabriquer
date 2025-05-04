#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in int lights;
layout(location = 4) in float ao;

out vec2 UV;
out vec3 vNormal;
out vec3 pos;
flat out int vLights;
out float vAO;

void main(){

        // TODO : Output position of the vertex, in clip space : MVP * position
        gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertices_position_modelspace,1);
        UV = vertexUV;
        
        vNormal = mat3(transpose(inverse(ModelMatrix))) * vertexNormal_modelspace;


        pos = vec3(ModelMatrix * vec4(vertices_position_modelspace,1));
        //pos = vec3(1.f);

        //camPos = vec3(0.f, 0.f, 0.f);

        vLights = lights;

        vAO = ao;
}

