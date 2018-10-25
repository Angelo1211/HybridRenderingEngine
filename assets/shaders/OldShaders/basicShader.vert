#version 450 core

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
    vec3 fragPos_tS;
    vec3 cameraPos_tS;
    vec3 dirLight_tS;
    vec3 pointLight_tS[4];
    vec3 fragPos_wS;
    vec3 cameraPos_wS;
    vec2 texCoord;
    vec4 fragPos_lS;
} vs_out;

//Matrices
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 lightSpaceMatrix;

//Camera data
uniform vec3 cameraPos_wS;

//Light data
uniform vec3 dirLight_wS;
uniform vec3 pointLight_wS[4];

void main(){
    //Position in clip space
    gl_Position = MVP*vec4(vertexPos_mS, 1.0); 

    //Passing texture coords
    vs_out.texCoord = aTexCoord;

    //World space output    
    // vec3 fragPos_wS  =  vec3(M * vec4(vertexPos_mS, 1.0));
    vs_out.fragPos_wS    =  vec3(M * vec4(vertexPos_mS, 1.0));
    vs_out.cameraPos_wS  =  cameraPos_wS;

    //Generating tangent matrix
    vec3 T = normalize(mat3(M) * tangent_tS);
    vec3 N = normalize(mat3(M) * normal_mS);
    T = normalize(T - dot(T, N ) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    //TangentSpaceOutput
    // vs_out.fragPos_tS = TBN * fragPos_wS;
    vs_out.fragPos_tS = TBN * vs_out.fragPos_wS;
    vs_out.cameraPos_tS = TBN * cameraPos_wS;
    vs_out.dirLight_tS = TBN * dirLight_wS;
    for(int i = 0; i < 4; ++i){
        vs_out.pointLight_tS[i] = TBN * pointLight_wS[i];
    }
    
    //Lights space output
    // vs_out.fragPos_lS  = lightSpaceMatrix * vec4(fragPos_wS, 1.0);
    vs_out.fragPos_lS  = lightSpaceMatrix * vec4(vs_out.fragPos_wS, 1.0);
}