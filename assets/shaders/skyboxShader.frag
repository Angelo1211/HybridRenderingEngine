#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main(){

    FragColor = texture(skybox, TexCoords);
    // FragColor = textureLod(skybox, TexCoords, 3.7);
}