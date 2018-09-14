// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-08
// ===============================

//Includes
#include "model.h"
#include "geometry.h"
#include "glm/glm.hpp"

void Model::loadModel(std::string path){
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::draw(Shader shader){
    for(int i = 0; i < meshes.size(); ++i){
        meshes[i].draw(shader);
    }
}

void Model::update(const unsigned int deltaT){

}

glm::mat4 Model::getModelMatrix(){
    //Model matrix assembly
    modelMatrix = glm::mat4(1.0);
    modelMatrix = glm::translate(modelMatrix, modelParameters.translation);
    // modelMatrix = glm::rotate(modelMatrix, (float)start/5000.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, modelParameters.scaling);
    return modelMatrix; 
}

void Model::processNode(aiNode *node, const aiScene *scene){
    //Process all the node meshes
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh,scene));
    }

    //process all the node children recursively
    for(unsigned int i = 0; i < node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int > indices;
    std::vector<Texture> textures;
    //Process vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; ++i){
        //Process vertex positions, normals and texture coordinates
        Vertex vertex;
        glm::vec3 vector;

        //Process position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        //Process normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        //Process texture coords
        if( mesh->HasTextureCoords(0) ){ 
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
        }
        else{
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    //Process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; ++i){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; ++j){
            indices.push_back(face.mIndices[j]);
        }
    }
    //TODO Process material and texture info
    printf("Material index: %u \n", mesh->mMaterialIndex);
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    aiString name;
    material->Get(AI_MATKEY_NAME, name);
    printf("Material name: %s \n", name.C_Str());

    return Mesh(vertices, indices, textures);
}