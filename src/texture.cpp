// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-15
// ===============================

#include "texture.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::setupTexture(const std::string &filePath, bool sRGB){
    path = filePath;
    unsigned int ID;
    glGenTextures(1, &ID);

    int width, height, nComponents;
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nComponents, 0);
    if(data){
        GLenum format;
        GLenum internalFormat;
        if(nComponents ==  1){
            format = GL_RED;
            internalFormat = GL_RED;
        }
        else if(nComponents == 3){
            format = GL_RGB;
            if(sRGB){
                internalFormat = GL_SRGB;
            }
            else{
                internalFormat = GL_RGB;
            }
        }
        else if(nComponents == 4){
            format = GL_RGBA;
            if(sRGB){
                internalFormat = GL_SRGB_ALPHA;
            }
            else{
                internalFormat = GL_RGBA;
            }
        }

        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        printf("Texture failed to load at path: %s \n"  ,filePath.c_str() );
        stbi_image_free(data);
    }

    textureID = ID;
}

void CubeMap::loadCubeMap(const std::string &folderPath){
    std::string filePath = folderPath + "/";
    unsigned int ID;    
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    
    int width, height, nComponents;
    for(unsigned int i = 0; i < numSidesInCube ; ++i ){
        std::string currentFile = filePath + fileHandleForFaces[i];
        unsigned char *data = stbi_load(currentFile.c_str(), &width, &height, &nComponents, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            printf("Texture failed to load at path: %s \n", currentFile.c_str());
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    textureID = ID;
    path = filePath;
    type = "cubemap";
}

void CubeMap::loadCubeMap(const int shadow_width, const int shadow_height){

    unsigned int ID;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    for(unsigned int i = 0; i < numSidesInCube; ++i){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    textureID = ID;
    path = "";
    type = "cubemap";
}