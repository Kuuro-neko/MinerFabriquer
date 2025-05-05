#version 330 core
out vec4 FragColor;

uniform int u_isSelected;

void main()
{
    if (u_isSelected == 1) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);  // Blanc opaque pour le slot sélectionné
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 0.6);  // Blanc semi-transparent pour les autres
    }
}
