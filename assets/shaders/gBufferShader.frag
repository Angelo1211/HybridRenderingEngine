#version 450 core
//Naming scheme clarification
// mS = model Space
// vS = view Space
// wS = world Space
// tS = tangent Space

layout(location = 0) out vec3 positionBuffer;
layout(location = 1) out vec3 normalsBuffer;
layout(location = 2) out vec4 albedoSpecBuffer;

in VS_OUT{
    vec3 fragPos_wS;
    vec2 texCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D diffuse1;
uniform sampler2D specular1;
uniform sampler2D normal1;

void main(){
    //Storing albedo values in the first three channels of the third texture buffer
    albedoSpecBuffer = texture(diffuse1, fs_in.texCoords);
    if(albedoSpecBuffer.a < 0.5){
        discard;
    }

    //Storing world space position in the first texture buffer
    positionBuffer = fs_in.fragPos_wS;

    //Storing normal in world space in second texture buffer
    vec3 normal   =  texture(normal1, fs_in.texCoords).rgb;
    normalsBuffer = fs_in.TBN * normalize(normal * 2.0 - 1.0);

    //Storing specular intensity in the third buffer
    albedoSpecBuffer.a = texture(specular1, fs_in.texCoords).r;
}
