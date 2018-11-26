/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-24
*/

#include "cubeMap.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

//Initializing all static variables at compile time
const glm::mat4 CubeMap::captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 CubeMap::captureViews[18] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};
const std::string CubeMap::fileHandleForFaces[6] = { "right.jpg", "left.jpg", "top.jpg",
                                                     "bottom.jpg", "front.jpg", "back.jpg" };
const unsigned int CubeMap::numSidesInCube = 6;
Cube CubeMap::cubeMapCube;

//Traditional cubemap generation from 6 regular image files named according to the
//string array fileHandlesForFaces. Order comes frfom Opengl cubemap specification
void CubeMap::loadCubeMap(const std::string &folderPath){
    unsigned int ID;    
    int width, height, nComponents;
    std::string filePath = folderPath + "/";

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    
    for(unsigned int i = 0; i < numSidesInCube ; ++i ){
        std::string currentFile = filePath + fileHandleForFaces[i];
        unsigned char *data = stbi_load(currentFile.c_str(), &width, &height, &nComponents, 0);

        if(data){
            //Enums iteration based on OpenGL specification
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else{
            printf("Texture failed to load at path: %s \n", currentFile.c_str());
        }
        stbi_image_free(data);
    }

    //Texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    textureID = ID;
    path = filePath;
    type = "cubemap";
}

// If the cubemap is not loaded in the traditional manner, it is probably generated.
// It will be filled in later, either through an image that is transformed to the right format
// or generated in some shader.
void CubeMap::generateCubeMap(const int width, const int height, CubeMapType cubeType){
    unsigned int ID;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    switch(cubeType){
        case SHADOW_MAP:
            for (unsigned int i = 0; i < numSidesInCube; ++i){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_DEPTH_COMPONENT, width, height, 0,
                             GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

        case HDR_MAP:
            for (unsigned int i = 0; i < numSidesInCube; ++i){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB32F,
                             width, height, 0,
                             GL_RGB, GL_FLOAT, NULL);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;

        case PREFILTER_MAP:
            for (unsigned int i = 0; i < numSidesInCube; ++i){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB16F,
                             width, height, 0,
                             GL_RGB, GL_FLOAT, NULL);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //For the specular IBL component we use the mipmap levels to store increasingly
            //rougher representations of the environment. And then interpolater between those
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            maxMipLevels = 5;
        break;
    }

    textureID = ID;
    path = ""; //Generated so no file path
    type = "cubemap";
}

//For use in the diffuse IBL setup for now
void CubeMap::convolveCubeMap(const unsigned int environmentMap,const Shader &convolveShader){
    convolveShader.use();
    convolveShader.setInt("environmentMap", 0);
    convolveShader.setMat4("projection", captureProjection);
    
    glViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

    for(unsigned int i = 0; i < numSidesInCube; ++i){
        convolveShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawCube();
    }
    
}

//Specular IBL cubemap component of hte integral
void CubeMap::preFilterCubeMap(const unsigned int environmentMap,
                               const unsigned int captureRBO,const Shader &filterShader){

    filterShader.use();
    filterShader.setInt("environmentMap", 0);
    filterShader.setMat4("projection", captureProjection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

    //For each Mip level we have to pre-filter the cubemap at each cube face
    for( unsigned int mip = 0; mip < maxMipLevels; ++mip){
        //Mip levels are decreasing powers of two of the original resolution of the cubemap
        unsigned int mipWidth  = unsigned int( width  * std::pow(0.5f, mip));
        unsigned int mipHeight = unsigned int( height * std::pow(0.5f, mip));

        //The depth component needs to be resized for each mip level too
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        
        for(unsigned int i = 0; i < numSidesInCube; ++i){
            filterShader.setMat4("view", captureViews[i]);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   textureID, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            drawCube();
        }
    }
}

//Transform an equirectangular map to a six sided cubemap
void CubeMap::equiRectangularToCubeMap(const unsigned int equirectangularMap,
                                       const int resolution, const Shader &transformShader){
    transformShader.use();
    transformShader.setInt("equirectangularMap", 0);
    transformShader.setMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, equirectangularMap);
    glViewport(0, 0, resolution, resolution);

    for(unsigned int i = 0; i < numSidesInCube; i++){
        transformShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawCube();
    }
}

//This function seems kind of useless, I should check to see if it is being inlined away
//by the compiler and think of better ways to do this
void CubeMap::drawCube(){
    cubeMapCube.draw();
}