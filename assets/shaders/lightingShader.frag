#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D position;
uniform sampler2D normals;
uniform sampler2D albedoSpec;

void main(){
    vec3 albedo = texture(albedoSpec, TexCoords).rgb;
    FragColor = vec4(albedo, 1.0);
}