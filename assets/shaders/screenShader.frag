#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main(){
    FragColor = vec4(texture(screenTexture, TexCoords).r, 0.0, 0.0, 1.0);
}