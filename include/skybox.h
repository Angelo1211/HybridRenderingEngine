#ifndef SKYBOX_H
#define SKYBOX_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-24
PURPOSE      : Container for the skybox texture in two supported formats:
               equirectangular map and cubemap. 
SPECIAL NOTES: Is not compatible with a fully deferred pipeline yet. Low Priority.
*/

//Includes
#include "texture.h"
#include "cubeMap.h"
#include "shader.h"
#include "glm/glm.hpp"
#include <string>

struct Skybox{
    //Has special depth testing requirements
    void draw();

    //Setup functions for skybox mesh (VAO) and textures
    void setup(const std::string &skyboxName, bool isHDR, int resolution);

    //Transforming the equirecangular texture to a cubemap format for rendering
    void fillCubeMapWithTexture(const Shader &buildCubeMapShader); 

    unsigned int resolution;

    //Equirectangular map is not rendered, just an intermediate state
    Texture equirectangularMap;
    CubeMap skyBoxCubeMap;
};

#endif