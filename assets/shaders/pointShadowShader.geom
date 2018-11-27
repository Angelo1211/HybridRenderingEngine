#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

//Learnopengl trick to send the same vertex info to 6 simultaneous faces of a cubemap at once
//this will also be touched upon on the shadow map;ping rewrite 
void main(){
    //for each face in a cubemap
    for(int face = 0; face < 6; ++face){
        gl_Layer = face;
        //for each vertex on a triangle
        for(int i = 0; i < 3; ++i){
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}