#ifndef CUBEMAP_H 
#define CUBEMAP_H
/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-20
PURPOSE      : Load, generate and modify cubemaps for skybox, IBL or environment map use. 
SPECIAL NOTES: A cube mesh is initialized in the scene class which contains the main openGL
               drawCall can cube VBA. This is done at that point since the OpenGL context has
               already been created and no cubemap has been loaded yet. It also allows for one
               single cube to be shared between all instances of the cubemap class. A lot of 
               repetition in the cubemap setup, optimization opportunity? Low priority.
*/

//Includes
#include "texture.h"
#include "meshPrimitives.h"

enum CubeMapType{
    SHADOW_MAP,
    HDR_MAP,
    PREFILTER_MAP
};

struct CubeMap : public Texture{
    //Cubemap instantiation   
    void loadCubeMap(const std::string &folderPath);
    void generateCubeMap(const int width, const int height, CubeMapType cubeType);

    //Cubemap modifications
    void convolveCubeMap(const unsigned int environmentMap, const Shader &convolveShader);
    void preFilterCubeMap(const unsigned int environmentMap, const unsigned int captureRBO,const Shader &filterShader);
    void equiRectangularToCubeMap(const unsigned int equirectangularMap, const int resolution, const Shader &transformShader);

    //TODO::Remove, Redundant?
    void drawCube();

    //Useful in the Specular IBL component
    unsigned int maxMipLevels;

    //Static constants used in all cubemaps
    static Cube cubeMapCube;
    static const glm::mat4 captureViews[18];
    static const unsigned int numSidesInCube;
    static const glm::mat4 captureProjection;
    static const std::string fileHandleForFaces[6]; //Order from Opengl cubemap enums
};

#endif