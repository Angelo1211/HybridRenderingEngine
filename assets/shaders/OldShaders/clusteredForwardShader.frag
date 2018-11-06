#version 460 core
//Naming scheme clarification
// mS = model Space
// vS = view Space
// wS = world Space
// tS = tangent Space

layout(early_fragment_tests) in;

out vec4 FragColor;

in VS_OUT{
    vec3 fragPos_wS;
    vec2 texCoords;
    vec4 fragPos_lS;
    vec3 T;
    vec3 B;
    vec3 N;
    mat3 TBN;
} fs_in;

//Dir light uniform
struct DirLight{
    vec3 direction;
    vec3 color;
};
uniform DirLight dirLight;

//Textures to sample from
uniform sampler2D shadowMap;
uniform sampler2D diffuse1;
uniform sampler2D specular1;
uniform sampler2D normal1;

//Misc Uniforms
uniform vec3 cameraPos_wS;

//To be changed in the future..
#define SHADOW_CASTING_POINT_LIGHTS 4 
//PointLight buffer in GPU
struct PointLight{
    vec4 position;
    vec4 color;
    bool enabled;
    float intensity;
    float range;
};

struct LightGrid{
    uint offset;
    uint count;
};

layout (std430, binding = 2) buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
    uvec2 screenDimensions;
    float scale;
    float bias;
};
layout (std430, binding = 3) buffer lightSSBO{
    PointLight pointLight[];
};

layout (std430, binding = 4) buffer lightIndexSSBO{
    uint globalLightIndexList[];
};

layout (std430, binding = 5) buffer lightGridSSBO{
    LightGrid lightGrid[];
};

//TODO:: Probably should be a buffer...
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

//TODO: change far plane to a different location
uniform samplerCube depthMaps[SHADOW_CASTING_POINT_LIGHTS];
uniform float far_plane;
uniform float zFar;
uniform float zNear;

//Function prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, float spec, float shadow);
vec3 calcPointLight(uint index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, float spec, float viewDistance);
float calcPointLightShadows(samplerCube depthMap, vec3 fragPos, float viewDistance);
float calcDirShadow(vec4 fragPosLightSpace);
float linearDepth(float depthSample);
uint  findSlice(float depth);

void main(){
    //Texture Reads
    vec3 color =  texture(diffuse1, fs_in.texCoords).rgb;
    float specularIntensity =  texture(specular1, fs_in.texCoords).r;
    vec3 norm   =  normalize(2.0 * texture(normal1, fs_in.texCoords).rgb - 1.0);

    //Components common to all light types
    mat3 TBN = mat3(fs_in.T, fs_in.B, fs_in.N);
    norm          = normalize(TBN * norm ); //going -1 to 1
    vec3 viewDir   = normalize(cameraPos_wS - fs_in.fragPos_wS);
    vec3 result    = vec3(0.0);

    //Locating which cluster you are a part of
    uint zTile     = uint(max(log2(linearDepth(gl_FragCoord.z)) * scale + bias, 0));
    uvec3 tiles    = uvec3( uvec2( gl_FragCoord.xy / tileSizes[3] ), zTile);
    uint tileIndex = tiles.x +
                     tileSizes.x * tiles.y +
                     (tileSizes.x * tileSizes.y) * tiles.z;  

    // shadow calcs
    float shadow = calcDirShadow(fs_in.fragPos_lS);
    float viewDistance = length(cameraPos_wS - fs_in.fragPos_wS);

    // //Directional light 
    result = calcDirLight(dirLight, norm, viewDir, color, specularIntensity, shadow) ;

    // Point lights
    uint lightCount       = lightGrid[tileIndex].count;
    uint lightIndexOffset = lightGrid[tileIndex].offset;

    for(uint i = 0; i < lightCount; i++){
        uint bigAssLightVectorIndex = globalLightIndexList[lightIndexOffset + i];
        result += calcPointLight(bigAssLightVectorIndex, norm, fs_in.fragPos_wS, viewDir, color, specularIntensity, viewDistance);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 col, float spec, float shadow){
    //Ambient component 
    // vec3 ambient  = 0.05 * col;
    float ambient  = 0.05;

    //Diffuse component
    vec3 lightDir = normalize(-light.direction);
    float diff    = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse  = light.color * diff;
    // float diffuse  =  diff;

    //Specular component
    vec3 halfway  = normalize(lightDir + viewDir);
    // float nDotHBP = max(dot(normal, halfway), 0.0); //N dot H using blinn phong
    float nDotHBP = pow(max(dot(normal, halfway), 0.0), 128.0); //N dot H using blinn phong
    // float nDotHBP = pow(max(dot(normal, halfway), 0.0), 128.0); //N dot H using blinn phong
    // vec3 specular = light.color * nDotHBP * spec;
    float specular =  nDotHBP * spec;

    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * col;
    vec3 lighting = (ambient + (1.0 - shadow) * light.color *  (diff + specular)) * col;
    
    //Total contribution
    return lighting;
}

vec3 calcPointLight(uint index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 col, float spec, float viewDistance){
    vec3 position = pointLight[index].position.xyz;
    vec3 color    = pointLight[index].color.rgb;
    //Attenuation calculation that is applied to all
    float distance = length(position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.07 * distance + 0.017 * (distance * distance));
    //ambient component
    // vec3 ambient = 0.005 * col;
    float ambient = 0.005;

    //diffuse component
    vec3 lightDir = normalize(position - fragPos);
    float nDotL   = clamp(dot(lightDir, normal), 0.0, 1.0);
    float diffuse  = nDotL ;

    //specular component
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotHBP = pow(max(dot(normal, halfway), 0.0), 128.0); //N dot H using blinn phong
    float specular = nDotHBP * spec;

    // //shadow stuff
    vec3 fragToLight = fragPos - position;

    float shadow = calcPointLightShadows(depthMaps[index], fragToLight, viewDistance);
    
    //total contibution 
    // return  attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
    return  attenuation * (ambient + (1.0 - shadow) * color * (diffuse + specular)) * col;
}

float calcDirShadow(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = 0.0;
    int   samples = 9;
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int i = 0; i < samples; ++i){
        float pcfDepth = texture(shadowMap, projCoords.xy + sampleOffsetDirections[i].xy * texelSize).r;
        shadow += projCoords.z - bias > pcfDepth ? 0.111111 : 0.0;
    }
    // for(int x = -1; x <= 1; ++x){
    //     for(int y = -1; y <= 1; ++y){
    //         float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
    //         shadow += projCoords.z - bias > pcfDepth ? 0.111111 : 0.0;
    //     }
    // }
    
    // shadow /= 9.0;

    return shadow;
}

float calcPointLightShadows(samplerCube depthMap, vec3 fragToLight, float viewDistance){
    float shadow      = 0.0;
    float bias        = 0.0;
    int   samples     = 8;
    float fraction    = 1.0/float(samples);
    float diskRadius  = (1.0 + (viewDistance / far_plane)) / 25.0;
    float currentDepth = (length(fragToLight) - bias);

    for(int i = 0; i < samples; ++i){
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i], diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth > closestDepth){
            shadow += fraction;
        }
    }
    // shadow /= float(samples);

    return shadow;
}

float linearDepth(float depthSample){
    float depthRange = 2.0 * depthSample - 1.0;
    // Near... Far... wherever you are...
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    // float linear = 2.0; 
    return linear;
}
