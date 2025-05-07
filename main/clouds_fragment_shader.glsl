#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D cloudTexture;
uniform vec3 cloudColor = vec3(1.0); // white cloud
uniform float cloudAlpha = 0.5;

uniform float scrollSpeed = 0.1; // speed of cloud movement
uniform float time; // time uniform for animation

uniform vec3 playerPos; // player's position
uniform float playerSpeed;

float fmod(float a, float b) {
    return a - b * floor(a / b);
}

void main() {
    // Cancel player movement if speed > 0 (i.e., the player is moving)
    vec2 playerMotion = vec2(-playerPos.x, playerPos.z) * playerSpeed;
    vec2 cloudScrollOffset = vec2(0.0, time * scrollSpeed);

    // Final UV: static clouds by subtracting player's offset
    vec2 scrollUV = vUV + cloudScrollOffset - playerMotion * scrollSpeed * 0.5;

    // Wrap UVs using fmod
    float U = fmod(scrollUV.x, 1.0);
    float V = fmod(scrollUV.y, 1.0);

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
