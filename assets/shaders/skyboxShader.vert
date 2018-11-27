#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 VP;

void main(){
    TexCoords = aPos;
    vec4 pos = VP * vec4(aPos, 1.0);
    //trick to have the z position always equal to one 
    //because during perspective division, everything will be
    //divided by the homogenous component w and if z = w, then w/w = 1
    gl_Position = pos.xyww;
}