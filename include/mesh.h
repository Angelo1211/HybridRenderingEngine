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

typedef std::unordered_map<std::string, Texture>  tAtlas;

//Beware the tale of the circular dependency!!!!
struct Vertex{
   glm::vec3 position; 
   glm::vec3 normal; 
   glm::vec3 tangent; 
   glm::vec3 biTangent; 
   glm::vec2 texCoords; 
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int > indices;
        std::vector<unsigned int > textures;
        Mesh(const std::vector<Vertex> &vertices, 
             const std::vector<unsigned int> &indices,
             const std::vector<unsigned int> &textures){
            this->vertices = vertices;
            this->indices  = indices;
            this->textures = textures;

            setupMesh();
        }

        void draw(const Shader &shader, const tAtlas &textureAtlas, bool textured);
    private:
        Texture currentTex;
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};

class Quad {
    public:
        void draw(const unsigned int readTexture1, const unsigned int readTexture2 = 0, const unsigned int computeTexture = 0);
        void drawDeffered(const unsigned int position,
                          const unsigned int normals,
                          const unsigned int albedoSpec);
        void setupQuad();

    private:
        unsigned int VAO, VBO;
};


#endif