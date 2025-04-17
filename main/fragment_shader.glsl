#version 330 core

uniform sampler2D TextureSampler;

in vec2 UV;
out vec4 color;
in vec3 vNormal;

uniform int displayNormals;

void main(){
        color = texture(TextureSampler, UV);
        if (displayNormals == 1) color = vec4(vNormal * 0.5 + 0.5, 1.0);
}
