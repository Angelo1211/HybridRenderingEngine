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
#include <string>


class Skybox{
    public:
        Skybox(const std::string &filePath){
            setupVertices();

            skyBoxCubeMap.loadCubeMap(filePath);
        };

        void draw();
        // void draw(const Shader &shader);

    private:
        void setupVertices();
        unsigned int VAO, VBO;

        CubeMap skyBoxCubeMap;
};

#endif