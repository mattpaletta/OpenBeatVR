#version 330 core
out vec4 FragColor;

uniform vec3 mineColour;

void main() {
    FragColor = vec4(mineColour, 1.0);
}