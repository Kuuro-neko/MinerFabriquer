#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform mat4 u_model;
uniform vec2 u_resolution;
out vec2 vUV;

void main()
{
    vec2 pos = (u_model * vec4(aPos, 0.0, 1.0)).xy;

    // Convertir coordonnées pixel en Normalized Device Coordinates
    vec2 ndc = pos / u_resolution * 2.0 - 1.0;
    // Inverser l'axe Y car OpenGL ...
    ndc.y = -ndc.y;

    gl_Position = vec4(ndc, 0.0, 1.0);

    vUV = aTexCoords;
}
