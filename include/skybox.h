#ifndef SKYBOX_H
#define SKYBOX_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-24
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

#include "texture.h"
#include "shader.h"
#include "glm/glm.hpp"
#include <string>

class Skybox{
    public:
        void setup(const std::string &filePath);

        void update();

        void draw();

    private:
        void setupVertices();
        unsigned int VAO, VBO;

        CubeMap skyBoxCubeMap;
};

#endif