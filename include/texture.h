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

struct Texture{
    void setupTexture(const std::string &filePath, bool sRGB);
    unsigned int textureID;
    std::string type;
    std::string path;
};

struct CubeMap : Texture{
    void loadCubeMap(const std::string &filePath);
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