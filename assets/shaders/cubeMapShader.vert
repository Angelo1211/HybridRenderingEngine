#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

//Very simple shader used in all cubemap related things
void main(){
    localPos = aPos;
    gl_Position = projection * view * vec4(localPos, 1.0);
}