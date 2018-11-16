#version 460 core

in vec2 TexCoords;

uniform sampler2D albedo;

void main(){
    float alpha = texture(albedo, TexCoords).a;
    if(alpha < 0.5){
        discard;
    }
    // gl_FragDepth = gl_FragCoord.z;
}