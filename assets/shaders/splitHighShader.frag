#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

//absolutely could be a compute shader..!
void main(){
    vec3 color = texture(screenTexture, TexCoords).rgb;
    //these values for the vec3 come from how our eyes naturally see
    //grayscale values. Something to do with color theory, but I'm not 100% sure
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    //high band pass filter
    if( brightness >= 1.0 ){
        FragColor = vec4(color, 1.0);
    }
    else
    {
        FragColor = vec4(vec3(0.0), 1.0);
    }
}