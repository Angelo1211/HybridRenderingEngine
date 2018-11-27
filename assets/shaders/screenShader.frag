#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform int offset; 
uniform float exposure;

void main(){

    //Merging both bloom textures into one hrdColor value before tonemapping
    vec3 hdrCol = texture(screenTexture, TexCoords).rgb;
    vec3 bloomCol = texture(bloomBlur, TexCoords).rgb;
    bloomCol -= hdrCol; // so I'm not sure if this is the correct way to do this, but I thought that we don't actually want to blur all the values, just the ones that are above one
    bloomCol = max(bloomCol, 0.0);//making sure we don't get any negative colors 
    hdrCol += bloomCol;

    //Exposure tone mapping
    vec3 toneMappedResult = vec3(1.0) - exp(-hdrCol * exposure);

    //reinhard tone mapping
    // vec3 toneMappedResult = hdrCol / (hdrCol + vec3(1.0));

    FragColor = vec4(toneMappedResult, 1.0) ;
}