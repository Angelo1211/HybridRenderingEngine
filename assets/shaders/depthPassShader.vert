#version 460 core
layout (location = 0 ) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 MVP;

void main(){
    gl_Position = MVP * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}