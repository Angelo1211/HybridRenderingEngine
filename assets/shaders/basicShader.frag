#version 450 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D ourTexure;

void main()
{
    FragColor = texture(ourTexure, texCoord);
}