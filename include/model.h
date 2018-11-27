#ifndef MODEL_H
#define MODEL_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-08
PURPOSE      : Container for the different meshes in a scene. Also serves as the loading point for mesh files 
               using assimp as the loading library. It also keeps track of and objects position and loads the 
               associated textures for each mesh that it contains.
SPECIAL NOTES: Because it uses assimp to load meshes it is naturally shaped by it in the way it loads and handles 
               data. Currently, all meshes load textures in the exact same way and therefore are limited to effectively
               having just one material type. This is a high priority change, probably the next version after the alpha.
*/

//Includes
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <unordered_map>
#include <string>

struct TransformParameters{
    TransformParameters(): scaling(glm::vec3(1.0f)) {};
    glm::vec3 translation;
    float angle;
    glm::vec3 rotationAxis;
    glm::vec3 scaling;
};

struct Model {
    Model(const std::string meshPath, const TransformParameters initParameters, bool IBL) : IBL(IBL){
        loadModel(meshPath);
        modelMatrix = glm::mat4(1.0);
        modelMatrix = glm::translate(modelMatrix, initParameters.translation);
        modelMatrix = glm::rotate(modelMatrix, initParameters.angle, initParameters.rotationAxis);
        modelMatrix = glm::scale(modelMatrix, initParameters.scaling);
    }
    //Base interface
    void loadModel(std::string path);
    void update(const unsigned int deltaT);
    void draw(const Shader &shader, const bool textured);

    //Model processing/loading functions
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<unsigned int> processTextures(const aiMaterial *material);

    //Object to world space matrix
    bool IBL;
    glm::mat4 modelMatrix;
    std::vector<Mesh> meshes; //Does it need to be a vector after initialization?

    //To avoid textures being loaded from disk more than once they are indexed into a dictionary
    std::unordered_map<std::string, Texture> textureAtlas;
    std::string directory, fileExtension;
};

#endif 