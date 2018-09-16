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
    void setupTexture(std::string filePath);
    unsigned int textureID;
    std::string type;
    std::string path;
};

#endif