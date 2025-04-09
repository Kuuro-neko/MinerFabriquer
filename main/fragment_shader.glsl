#version 330 core

uniform sampler2D TextureSampler;

in vec2 UV;
out vec4 color;


void main(){
        color = texture(TextureSampler, UV);
}
