#version 450 core
out vec4 FragColor;

in VS_OUT{
    vec3 fragPos_wS;
    vec3 normal_wS;
    vec2 texCoord;
} fs_in;

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight{
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

//Camera data
uniform vec3 cameraPos_wS;

//Textures to sample from
uniform sampler2D diffuse1;
uniform sampler2D specular1;

//Function prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, vec3 spec);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, vec3 spec);

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

    //Directional light 
    result = calcDirLight(dirLight, norm, viewDir, color, specularIntensity) ;

    //Point lights
    for(int i = 0; i < POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], norm, fs_in.fragPos_wS, viewDir, color, specularIntensity);
    }

    FragColor = clamp(vec4(result, 1.0), 0.0, 1.0);
    // FragColor = vec4(specular, 1.0);
    // FragColor = vec4(diffuse, 1.0);
    // FragColor = vec4( ambient, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, vec3 spec){

    //Ambient component 
    vec3 ambient  = light.ambient * col;

    //Diffuse component
    vec3 lightDir = normalize(-light.direction);
    float nDotL   = clamp(dot(lightDir, normal), 0.0, 1.0);
    vec3 diffuse  = light.diffuse * nDotL * col;

    //Specular component
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotHBP = pow(max(dot(normal, halfway), 0.0), 128.0); //N dot H using blinn phong
    vec3 specular = light.specular * nDotHBP * spec;
    
    //Total contribution
    return(specular + ambient + diffuse);
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

    //total contibution 
    return  attenuation * (specular + ambient + diffuse);
}

