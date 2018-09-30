#version 450 core
out vec4 FragColor;

in VS_OUT{
    vec3 fragPos_wS;
    vec3 normal_wS;
    vec2 texCoord;
    vec4 fragPos_lS;
} fs_in;

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight{
    samplerCube depthMap;
    vec3 position;

    float constant;
    float linear; 
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define POINT_LIGHTS 4 
uniform PointLight pointLights[POINT_LIGHTS];
uniform float far_plane;

//Camera data
uniform vec3 cameraPos_wS;

//Textures to sample from
uniform sampler2DShadow shadowMap;

uniform sampler2D diffuse1;
uniform sampler2D specular1;

//Function prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, vec3 spec, float shadow);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, vec3 spec);
float calcShadows(vec4 fragPosLightSpace);
// float calcPointLightShadows(vec4 fragPosLightSpace);

void main(){
    //Texture Reads
    vec4 albedo =  texture(diffuse1, fs_in.texCoord);
    if(albedo.a < 0.5){
        discard;
    }
    vec3 color = albedo.rgb; 
    vec3 specularIntensity =  vec3(texture(specular1, fs_in.texCoord).r);

    //Components common to all light types
    vec3 norm     = normalize(fs_in.normal_wS);
    vec3 viewDir  = normalize(cameraPos_wS - fs_in.fragPos_wS);
    vec3 result   = vec3(0.0);

    //shadow calcs
    float shadow = calcShadows(fs_in.fragPos_lS);

    //Directional light 
    result = calcDirLight(dirLight, norm, viewDir, color, specularIntensity, shadow) ;

    // Point lights
    for(int i = 0; i < POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], norm, fs_in.fragPos_wS, viewDir, color, specularIntensity);
    }

    FragColor = clamp(vec4(result, 1.0), 0.0, 1.0);
    // FragColor = vec4(specular, 1.0);
    // FragColor = vec4(diffuse, 1.0);
    // FragColor = vec4( ambient, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, vec3 spec, float shadow){

    //Ambient component 
    vec3 ambient  = light.ambient * col;

    //Diffuse component
    vec3 lightDir = normalize(-light.direction);
    float nDotL   = clamp(dot(lightDir, normal), 0.0, 1.0);
    vec3 diffuse  = light.diffuse * nDotL * col;

    //Specular component
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotHBP = pow(max(dot(normal, halfway), 0.0), 32.0); //N dot H using blinn phong
    vec3 specular = light.specular * nDotHBP * spec;

    vec3 lighting = (ambient + (shadow) * (diffuse + specular)) * col;
    
    //Total contribution
    return lighting;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, vec3 spec){

    //Attenuation calculation that is applied to all
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                               light.linear * distance +
                               light.quadratic * (distance * distance));

    //ambient component
    vec3 ambient = light.ambient * col;

    //diffuse component
    vec3 lightDir = normalize(light.position - fragPos);
    float nDotL   = clamp(dot(lightDir, normal), 0.0, 1.0);
    vec3 diffuse  = light.diffuse * nDotL * col;

    //specular component
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotHBP = pow(max(dot(normal, halfway), 0.0), 128.0); //N dot H using blinn phong
    vec3 specular = light.specular * nDotHBP * spec;

    //shadow stuff
    vec3 fragToLight = fragPos - light.position;
    float closestDepth = texture(light.depthMap, fragToLight).r;

    closestDepth *= 2000.0;

    float currentDepth = length(fragToLight);

    float bias = 0.05;

    float shadow = currentDepth - bias > closestDepth ? 1.0 :0.0;

    //total contibution 
    return  attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

float calcShadows(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float shadow = texture(shadowMap, projCoords.xyz);
    return shadow;
}

