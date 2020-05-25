#version 330 core
out vec4 FragColor;

// in vec2 TexCoord;

uniform vec3 wallColour;

// uniform sampler2D texture1;

void main() {
    FragColor = vec4(wallColour, 1.0); ;
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
