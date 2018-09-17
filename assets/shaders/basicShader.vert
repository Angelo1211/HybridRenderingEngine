#version 450 core
layout (location = 0) in vec3 vertexPos_mSpace; // the position variable has attribute position 0
layout (location = 1) in vec3 normal_mSpace;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT{
    vec3 viewDir_vSpace;
    vec3 lightDir_vSpace;
    vec3 normal_vSpace;
    vec2 texCoord;
} vs_out;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 V;

void main(){
    //Position in clip space
    gl_Position = MVP*vec4(vertexPos_mSpace, 1.0); 
    
    //ViewDirection in view space
    vec3 vertexPos_vSpace  =  vec3(MV * vec4(vertexPos_mSpace, 1.0));
    vs_out.viewDir_vSpace  = -vertexPos_vSpace;

    //LightDirection in view space
    vec3 lightPosition_vSpace = vec3(V * vec4(1.0, 1.0, 1.0, 1.0));
    vs_out.lightDir_vSpace    = lightPosition_vSpace - vertexPos_vSpace;

    //Normal in view space
    vs_out.normal_vSpace   = vec3( MV * vec4(normal_mSpace, 0.0));

    //Passing texture coords
    vs_out.texCoord = aTexCoord;
}