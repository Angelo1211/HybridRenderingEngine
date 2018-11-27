#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 M;

//world space position
void main(){
    gl_Position = M * vec4(aPos, 1.0);
}