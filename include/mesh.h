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
#include "shader.h"
#include "texture.h"
#include <vector>
#include <string>
#include <unordered_map>

//Beware the tale of the circular dependency!!!!
struct Vertex{
   glm::vec3 position; 
   glm::vec3 normal; 
   glm::vec2 texCoords; 
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int > indices;
        std::vector<std::string> textures;
        Mesh(const std::vector<Vertex> &vertices, 
             const std::vector<unsigned int> &indices,
             const std::vector<std::string> &textures){
            this->vertices = vertices;
            this->indices  = indices;
            this->textures = textures;

            setupMesh();
        }

        void draw(const Shader &shader, const std::unordered_map<std::string, Texture> &textureAtlas);
    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};


#endif