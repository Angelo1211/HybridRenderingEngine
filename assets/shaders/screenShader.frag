#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D computeTexture;
uniform int offset; 
uniform float exposure;

void main(){
    // float frequency = 100;
    // float amplitude = 0.001;
    // float speed = 0.01;
    // float distortion = sin(TexCoords.y * frequency + offset*speed)* amplitude;
    // vec2 offSetCoords = vec2(TexCoords.x + distortion, TexCoords.y);
    // FragColor = texture(screenTexture, offSetCoords);

    vec3 hdrCol = texture(screenTexture, TexCoords).rgb;
    vec3 bloomCol = texture(bloomBlur, TexCoords).rgb;
    vec3 computeCol = texture(computeTexture, TexCoords).rgb;
    ivec2 linCoords = ivec2(TexCoords.x * 16, TexCoords.y * 8);
    // vec3 tileColor = tiles[linCoords.x][linCoords.y].rgb;

    // hdrCol += bloomCol + tileColor;
    hdrCol += bloomCol;

    //Exposure tone mapping
    vec3 toneMappedResult = vec3(1.0) - exp(-hdrCol * exposure);
    //reinhard tone mapping
    // vec3 toneMappedResult = result / (result + vec3(1.0));

    FragColor = vec4(toneMappedResult, 1.0) ;
    // FragColor  = vec4(computeCol, 1.0);
    // FragColor = vec4(result, 1.0) ;
    // FragColor = vec4(vec3(texture(screenTexture, TexCoords).r), 1.0);
}