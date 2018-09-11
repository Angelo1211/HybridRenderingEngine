#ifndef MESH_H
#define MESH_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-10
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Headers
#include "glm/glm.hpp"
#include "geometry.h"
#include "shader.h"
#include <vector>

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int > indices;
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices){
            this->vertices = vertices;
            this->indices  = indices ;

            setupMesh();
        }

        void draw(Shader shader);
    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};

#endif