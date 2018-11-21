#ifndef MESH_H
#define MESH_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-10
PURPOSE      : Main container for geometric data. The vertex is the smallest unit of geometric data.
               This class is tasked with creating the OpenGL representation of the loaded data.
SPECIAL NOTES: Specific primitive meshes are defined in the meshprimitives.h file. Currently, the mesh
               also acts as a container for the texture indices of hte screen. This is limiting
               since it forces all meshes to have a similar texture representation. This shall be
               adressed with a re-write of the mesh/model system that shall introduce a material
               class that will take care of the textures and shaders used in the mesh. High priority.
*/

//Headers
#include "glm/glm.hpp"
#include "shader.h"
#include "texture.h"
#include <vector>
#include <string>

struct Vertex{
   glm::vec3 position; 
   glm::vec3 normal; 
   glm::vec3 tangent; 
   glm::vec3 biTangent; 
   glm::vec2 texCoords; 
};

struct Mesh {
    //Supposedly STL is not recommended for performance, but I don't believe this is the case
    //in our current implementation
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> textures;

    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices,
         const std::vector<unsigned int> &textures)
    {
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;

        setupMesh();
    }

    void setupMesh();
    void draw(const Shader &shader, bool textured);

    //OpenGL drawing variables
    unsigned int VAO, VBO, EBO;
};



#endif