#version 430 core

//Naming scheme clarification
// mS = model Space
// vS = view Space
// wS = world Space
// tS = tangent Space

layout (location = 0) in vec3 vertexPos_mS; // the position variable has attribute position 0
layout (location = 1) in vec3 normal_mS;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent_tS;
layout (location = 4) in vec3 biTangent_tS;

out VS_OUT{
    vec3 fragPos_wS;
    vec2 texCoords;
    vec4 fragPos_lS;
    vec3 T;
    vec3 B;
    vec3 N;
    mat3 TBN;
} vs_out;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 lightSpaceMatrix; // Technically this only allows for one directional light, but will do for now

void main(){
    //Position in clip space
    gl_Position = MVP*vec4(vertexPos_mS, 1.0); 

    //Passing texture coords
    vs_out.texCoords = aTexCoord;

    //World Space fragment position
    vs_out.fragPos_wS    =  mat3(M) * vertexPos_mS;

    //Generating tangent matrix
    vs_out.T = normalize(mat3(M) * tangent_tS);
    vs_out.B = normalize(mat3(M) * biTangent_tS);
    vs_out.N = normalize(mat3(M) * normal_mS);

    //Lights space output
    vs_out.fragPos_lS  = lightSpaceMatrix * vec4(vs_out.fragPos_wS, 1.0);
}
