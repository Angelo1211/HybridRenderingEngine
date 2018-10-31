#version 460 core

//Naming scheme clarification
// mS = model Space
// vS = view Space
// wS = world Space
// tS = tangent Space

layout (location = 0) in vec3 vertexPos_mS; // the position variable has attribute position 0
layout (location = 1) in vec3 normal_mS;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent_tS;

out VS_OUT{
    vec3 fragPos_wS;
    vec2 texCoords;
    vec4 fragPos_lS;
    mat3 TBN;
} vs_out;

// flat out mat3 TBN;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 lightSpaceMatrix;

void main(){
    //Position in clip space
    gl_Position = MVP*vec4(vertexPos_mS, 1.0); 

    //Passing texture coords
    vs_out.texCoords = aTexCoord;

    //World Space fragment position
    vs_out.fragPos_wS    =  mat3(M) * vertexPos_mS;

    //Generating tangent matrix
    vec3 T = normalize(mat3(M) * tangent_tS);
    vec3 N = normalize(mat3(M) * normal_mS);
    T = normalize(T - dot(T, N ) * N);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);

    //Lights space output
    vs_out.fragPos_lS  = lightSpaceMatrix * vec4(vs_out.fragPos_wS, 1.0);
}