// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-15
// ===============================

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"
#include <glad/glad.h>

Texture::Texture(std::string filePath){
    path = filePath;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nComponents;
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nComponents, 0);
    if(data){
        GLenum format;
        if(nComponents ==  1){
            format = GL_RED;
        }
        else if(nComponents == 3){
            format = GL_RGB;
        }
        else if(nComponents == 4){
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        printf("Texture failed to load at path: %s",filePath.c_str() );
        stbi_image_free(data);
    }
}