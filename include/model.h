#ifndef MODEL_H
#define MODEL_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-10
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Includes
#include "shader.h"
#include "mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <string>

struct TransformParameters{
    TransformParameters(): scaling(glm::vec3(1.0f)) {};
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scaling;
};

class Model {
    public:
        Model(const std::string meshPath, const std::string materialPath, const TransformParameters initParameters){
            loadModel(meshPath);
            modelParameters = initParameters;
        }
        void draw(Shader shader);
        
        void update( const unsigned int deltaT);

        glm::mat4 getModelMatrix();
    private:
        TransformParameters modelParameters;
        glm::mat4 modelMatrix;

        std::vector<Mesh> meshes;        
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};

#endif 