#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

//Misc Uniforms
uniform vec3 cameraPos_wS;

//Gbuffer textures
uniform sampler2D position;
uniform sampler2D normals;
uniform sampler2D albedoSpec;

//Lighting Uniforms
struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    mat4 lightSpaceMatrix;
};
uniform DirLight dirLight;
uniform sampler2D shadowMap;

//TODO fix point light amount being fixed
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

vec3  calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, vec3 spec, float shadow);
float calcDirShadow(vec4 fragPosLightSpace);
vec3  calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, vec3 spec, float viewDistance);
float calcPointLightShadows(samplerCube depthMap, vec3 fragPos, float viewDistance);

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

void main(){
    //Texture Reads
    vec3 color =  texture(albedoSpec, TexCoords).rgb;
    vec3 specularIntensity =  vec3(texture(albedoSpec, TexCoords).a);
    vec3 norm = texture(normals, TexCoords).rgb;
    vec3 position = texture(position, TexCoords).rgb;

    //Common components
    vec3 viewDir = normalize(cameraPos_wS - position);
    vec3 result = vec3(0.0);

    //Shadow stuff
    float shadow = calcDirShadow(dirLight.lightSpaceMatrix * vec4(position, 1.0));
    float viewDistance = length(cameraPos_wS - position);

    //Directional lights
    result = calcDirLight(dirLight, norm, viewDir, color, specularIntensity, shadow);

    //Point Lights
    for(int i = 0; i < POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], norm, position, viewDir, color, specularIntensity, viewDistance);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, vec3 spec, float shadow){

    //Ambient component 
    vec3 ambient  = light.ambient * col;

    //Diffuse component
    vec3 lightDir = normalize(-light.direction);
    float nDotL   = max(dot(lightDir, normal), 0.0);
    vec3 diffuse  = light.diffuse * nDotL * col;

    //Specular component
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotHBP = pow(max(dot(normal, halfway), 0.0), 128.0); //N dot H using blinn phong
    vec3 specular = light.specular * nDotHBP * spec;

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * col;
    
    //Total contribution
    return lighting;
}

float calcDirShadow(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = 0.0;
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x){
        for(int y = -1; y <= 1; ++y){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    
    shadow /= 9.0;

    return shadow;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, vec3 spec, float viewDistance){

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

    float shadow = calcPointLightShadows(light.depthMap, fragToLight, viewDistance);
    
    //total contibution 
    return  attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

float calcPointLightShadows(samplerCube depthMap, vec3 fragToLight, float viewDistance){
    float shadow      = 0.0;
    float bias        = 0.0;
    int   samples     = 20;
    float diskRadius  = (1.0 + (viewDistance / far_plane)) / 25.0;
    float currentDepth = length(fragToLight);

    for(int i = 0; i < samples; ++i){
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i], diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth){
            shadow += 1;
        }
    }
    shadow /= float(samples);

    return shadow;
}