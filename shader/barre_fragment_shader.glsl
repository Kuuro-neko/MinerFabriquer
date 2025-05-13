#version 330 core
out vec4 FragColor;

uniform sampler2D TextureSampler;

uniform int u_isSelected;
in vec2 vUV;
uniform vec2 u_uvOffset;

void main()
{
    if(u_uvOffset.x >= 0.0) {
        vec4 texColor = texture(TextureSampler, vUV + u_uvOffset);
        FragColor = vec4(texColor.r, texColor.g, texColor.b, 1.0);
        if (u_isSelected != 1) {
            FragColor = FragColor * vec4(vec3(0.5), 1.0); // Assombrir la texture des slots non sélectionnés
        }
    } else {
        if (u_isSelected == 1) {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);  // Blanc opaque pour le slot sélectionné
        } else {
            FragColor = vec4(1.0, 1.0, 1.0, 0.4);  // Blanc semi-transparent pour les autres
        }
    }
}
