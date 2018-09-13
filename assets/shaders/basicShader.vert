#version 450 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec2 aNormal;
layout (location = 1) in vec2 aTexCoord;
out vec2 texCoord;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP*vec4(aPos, 1.0); 
    texCoord = aTexCoord;
}