#version 450 core

//Naming scheme clarification
// mS = model Space
// vS = view Space
// wS = world Space

layout (location = 0) in vec3 vertexPos_mS; // the position variable has attribute position 0
layout (location = 1) in vec3 normal_mS;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT{
    vec3 fragPos_wS;
    vec3 normal_wS;
    vec2 texCoord;
} vs_out;

//Matrices
uniform mat4 MVP;
uniform mat4 M;

void main(){
    //Position in clip space
    gl_Position = MVP*vec4(vertexPos_mS, 1.0); 
    
    vs_out.fragPos_wS  =  vec3(M * vec4(vertexPos_mS, 1.0));

    vs_out.normal_wS   = mat3(M) * normal_mS ;

    //Passing texture coords
    vs_out.texCoord = aTexCoord;
}