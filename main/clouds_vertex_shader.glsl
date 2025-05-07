#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormals;

out vec2 vUV;
out vec3 vWorldPos;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
    vUV = aUV;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);

    vec4 worldPos = ModelMatrix * vec4(aPos, 1.0);
    vWorldPos = vec3(worldPos.x, worldPos.y, worldPos.z);
}
