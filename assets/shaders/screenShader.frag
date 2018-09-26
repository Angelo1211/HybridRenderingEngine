#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int offset; 

void main(){
    // float frequency = 100;
    // float amplitude = 0.001;
    // float speed = 0.01;
    // float distortion = sin(TexCoords.y * frequency + offset*speed)* amplitude;
    // vec2 offSetCoords = vec2(TexCoords.x + distortion, TexCoords.y);
    // FragColor = texture(screenTexture, offSetCoords);
    // FragColor = vec4(pow(texture(screenTexture, TexCoords).rgb, vec3(1/2.2)), 1.0);
    FragColor = texture(screenTexture, TexCoords);
}