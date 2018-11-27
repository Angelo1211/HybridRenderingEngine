#version 430 core

in vec2 TexCoords;

uniform sampler2D albedo;

void main(){
    //This kills perf but I have no idea of how else to alpha test the sponza leaves 
    float alpha = texture(albedo, TexCoords).a;
    if(alpha < 0.5){
        discard;
    }
}