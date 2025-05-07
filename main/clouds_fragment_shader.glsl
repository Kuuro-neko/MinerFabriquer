#version 330 core
in vec2 vUV;
out vec4 FragColor;
in vec3 vWorldPos;

uniform sampler2D cloudTexture;
uniform vec3 cloudColor = vec3(1.0); // white cloud
uniform float cloudAlpha = 0.5;

uniform float scrollSpeed = 0.1; // speed of cloud movement
uniform float time; // time uniform for animation

float fmod(float a, float b) {
    return a - b * floor(a / b);
}

void main() {
    vec2 worldUV = vec2(vWorldPos.x, vWorldPos.z) * 0.0003;
    worldUV += vec2(0.0, time * scrollSpeed);

    float U = fmod(worldUV.x, 1.0);
    float V = fmod(worldUV.y, 1.0);

    float baseAlpha = texture(cloudTexture, vec2(U, V)).r * cloudAlpha;

    // Normalize vUV from [0.0, 0.1] to [0.0, 1.0]
    vec2 uvNorm = vUV / 0.1;

    float falloff = 0.2;

    // Fade edges
    float edgeFade = smoothstep(0.0, falloff, uvNorm.x) *
                     smoothstep(0.0, falloff, uvNorm.y) *
                     smoothstep(0.0, falloff, 1.0 - uvNorm.x) *
                     smoothstep(0.0, falloff, 1.0 - uvNorm.y);

    float alpha = baseAlpha * edgeFade;

    FragColor = vec4(cloudColor, alpha);
}
