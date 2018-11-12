#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
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

    bloomCol -= hdrCol;
    bloomCol = max(bloomCol, 0.0);
    // if( dot(bloomCol, vec3(1.0)) > 0.0){
    //     hdrCol =  (hdrCol + bloomCol) * 0.5 ;
    // }
    hdrCol += bloomCol;

    //Exposure tone mapping
    vec3 toneMappedResult = vec3(1.0) - exp(-hdrCol * exposure);
    //reinhard tone mapping
    // vec3 toneMappedResult = hdrCol / (hdrCol + vec3(1.0));

    FragColor = vec4(toneMappedResult, 1.0) ;
    // FragColor = vec4(vec3(texture(screenTexture, TexCoords).r), 1.0);
}