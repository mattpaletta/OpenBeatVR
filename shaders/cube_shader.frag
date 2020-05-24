#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 cubeColour;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord) * vec4(cubeColour, 1.0); ;
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
