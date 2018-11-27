#version 430 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

//with the shadow mapping rewrite I need to rethink if this is the best approach
//now that we have cluster shading working
void main(){
    float lightDistance = length(FragPos.xyz - lightPos);

    lightDistance = lightDistance / far_plane; //getting that 0-1 range

    gl_FragDepth = lightDistance;
}