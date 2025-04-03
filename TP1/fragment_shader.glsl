#version 330 core

uniform sampler2D PlanetTextureSampler;
in vec2 UV;
// Ouput data
out vec3 color;
in float y;
uniform float ymax;
uniform float ymin;
void main(){
        color = texture(PlanetTextureSampler, UV).rgb;
}
