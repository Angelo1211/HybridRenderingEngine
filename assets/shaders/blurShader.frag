#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform bool horizontal; 
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main(){
    //Size of single texel
    vec2 tex_offset = 1.0 / textureSize(screenTexture, 0);
    //Contribution by current fragment
    vec3 result = texture(screenTexture, TexCoords).rgb * weight[0];
    if(horizontal){
        for(int i = 1; i < 5; ++i){
            result += texture(screenTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(screenTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else{
        for(int i = 1; i < 5; ++i){
            result += texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0) ;
}