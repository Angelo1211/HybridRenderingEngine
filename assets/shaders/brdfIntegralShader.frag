#version 430 core
out vec2 FragColor;
in vec2 TexCoords;

const uint SAMPLE_COUNT = 1024u;
const float PI= 3.14159265359;

float radicalInverse(uint bits);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float geometrySchlickGGX(float nDotV, float roughness);
vec2 hammersley(uint i, float invN);
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness);
vec2 integratedBRDF(float nDotV, float roughness);

//TODO:: definitely move this into a compute shader
void main(){
    FragColor =  integratedBRDF(TexCoords.x, TexCoords.y);
}

//TODO:: Good place for a blog post to explain this in detail?
vec2 integratedBRDF(float nDotV, float roughness){
    float invCount = 1.0/float(SAMPLE_COUNT);

    vec3 V;
    V.x = sqrt(1 - nDotV * nDotV); // sin
    V.y = 0.0;
    V.z = nDotV; // cos

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

      for(uint i =0; i < SAMPLE_COUNT; ++i){
        vec2 Xi = hammersley(i, invCount);
        vec3 H  = importanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float nDotL = max(L.z, 0.0);
        float nDotH = max(H.z, 0.0);
        float vDotH = max(dot(V, H), 0.0);
        float oneMinusVdotH = 1.0 - vDotH;

        if( nDotL > 0.0){
            float G = geometrySmith(N, V, L, roughness);
            float G_Vis = (G * vDotH) / ( nDotH * nDotV);
            float Fc = oneMinusVdotH * oneMinusVdotH * oneMinusVdotH * oneMinusVdotH * oneMinusVdotH;

            A += (1.0 - Fc ) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A *= invCount;
    B *= invCount;
    return vec2(A,B);
}

// taken from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radicalInverse(uint bits){
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 hammersley(uint i, float invN ){
    return vec2(float(i) * invN, radicalInverse(i));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness){
    float a = roughness * roughness;

    //hemisphere sample 
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta *cosTheta);

    //Spherical to cartesian
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    //Tangent space cartesian to world space
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    //Multiplaying halfway vector times implicit TBN matrix
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N* H.z;
    return normalize(sampleVec);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float nDotV = max(dot(N, V), 0.0);
    float nDotL = max(dot(N, L), 0.0);

    float ggx1  = geometrySchlickGGX(nDotV, roughness);
    float ggx2  = geometrySchlickGGX(nDotL, roughness);

    return ggx1 * ggx2;
}

float geometrySchlickGGX(float nDotV, float roughness){
    float a = roughness;
    float k = (a * a) * 0.5;

    float nom = nDotV;
    float denom = nDotV * (1.0 - k) + k;

    return nom / denom;
}