#version 450 core

out vec4 FragColor;

in vec2 texCoord;
in vec4 vertCol;

uniform sampler2D ourTexure;

void main()
{
    FragColor = texture(ourTexure, texCoord);
    // FragColor = vertCol; //vec4(1.0, 1.0, 1.0, 1.0);
}