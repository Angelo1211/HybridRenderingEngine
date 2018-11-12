#ifndef TEXTURE_H 
#define TEXTURE_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-15
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Includes
#include <string>
#include "shader.h"

enum CubeMapType{
    SHADOW_MAP,
    HDR_MAP
};

struct Texture{
    void setupTexture(const std::string &filePath, bool sRGB);
    void setupHDRTexture(const std::string &filePath);
    unsigned int  loadDSFile(char const* Filename);
    unsigned int textureID;
    int width, height, nComponents;
    std::string type;
    std::string path;
};

struct CubeMap : public Texture{
    void loadCubeMap(const std::string &filePath);
    void generateCubeMap(const int width, const int height, CubeMapType cubeType);
    void convolveCubeMap(const unsigned int environmentMap, const unsigned int cubeVAO, Shader *convolveShader);
    const unsigned int numSidesInCube = 6;

    //Order for this comes from the Opengl cubemap enums
    const std::string fileHandleForFaces[6] = {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };

};

#endif