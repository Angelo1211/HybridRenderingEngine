#version 450 core
out vec4 FragColor;

in VS_OUT{
    vec3 viewDir_vSpace;
    vec3 lightDir_vSpace;
    vec3 normal_vSpace;
    vec2 texCoord;
} fs_in;

uniform sampler2D diffuse1;
uniform sampler2D specular1;
// uniform sampler2D albedoTexture;

//Lighting consts
const float kD = 0.9;
const float kS = 0.4;

void main(){
    //comment comment
    vec3 color =  texture(diffuse1, fs_in.texCoord).rgb;
    vec3 specularIntensity =  vec3(texture(specular1, fs_in.texCoord).r);

    //Ambient component
    vec3 ambient = 0.05 * color;

    //diffuse
    vec3 lightDir = normalize(fs_in.lightDir_vSpace);
    vec3 norm     = normalize(fs_in.normal_vSpace);
    float diff    = clamp(dot(lightDir, norm), 0.0, 1.0);
    vec3 diffuse  = kD*diff*color;

    //specular
    vec3 viewDir  = normalize(fs_in.viewDir_vSpace);
    vec3 halfway  = normalize(lightDir + viewDir);
    float spec    = pow(max(dot(norm, halfway), 0.0), 128.0);
    vec3 specular = vec3(kS) * spec * specularIntensity;

    FragColor = vec4(specular + diffuse + ambient, 1.0);
    // FragColor = vec4(specular, 1.0);
    // FragColor = vec4(diffuse, 1.0);
    // FragColor = vec4( ambient, 1.0);
}