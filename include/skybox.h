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
#include "glm/gtc/matrix_transform.hpp"
#include <string>

struct Skybox{
    void setup(const std::string &skyboxName, bool isHDR, int resolution);

    void update();
    void draw();
    void setupVertices();

    void fillCubeMapWithTexture(Shader *buildCubeMapShader);
    unsigned int VAO, VBO;
    int resolution;
    Texture equirectangularMap;
    CubeMap skyBoxCubeMap;
};

#endif