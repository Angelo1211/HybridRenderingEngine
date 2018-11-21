/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-08
*/

//Includes
#include "model.h"
#include "geometry.h"
#include "glm/glm.hpp"
#include "fileManager.h"
#include <string>

void Model::loadModel(std::string path){
    Assimp::Importer importer;
    // const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate |aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes |aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

    fileExtension = FLOAD::getFileExtension(path);

    directory = path.substr(0, path.find_last_of('/'));
    directory += "/";
    processNode(scene->mRootNode, scene);

}

void Model::draw(const Shader &shader, const  bool textured){
    for(int i = 0; i < meshes.size(); ++i){
        meshes[i].draw(shader, textured);
    }
}

void Model::update(const unsigned int deltaT){

}

glm::mat4 Model::getModelMatrix(){
    //Model matrix assembly
    modelMatrix = glm::mat4(1.0);
    modelMatrix = glm::translate(modelMatrix, modelParameters.translation);
    modelMatrix = glm::rotate(modelMatrix, modelParameters.angle, modelParameters.rotationAxis);
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
    std::vector<unsigned int > textures;
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

        //Process tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;

        //Process biTangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.biTangent = vector;

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
            vertex.texCoords = vec; 
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
    //Process material and texture info
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    textures = processTextures(material);

    return Mesh(vertices, indices, textures);
}
/*
    This whole section needs to be remade badly
    I am kind of rushing here so I know this is bad
    But I have deadlines
    I must finish the PBR part before I can go back and fix this mesh
  */
std::vector<unsigned int> Model::processTextures(const aiMaterial *material){
    std::vector<unsigned int> textures;

    //Finding current texture directory
    aiString texturePath;
    aiTextureType type;
    std::string fullTexturePath;

    //Checking all texture stacks
    for(int tex = aiTextureType_NONE ; tex <= aiTextureType_UNKNOWN; tex++){
        type = static_cast<aiTextureType>(tex);
        fullTexturePath = directory;

        if( material->GetTextureCount(type) > 0 ){
            //We only care about the first texture assigned we don't expect multiple to be assigned
            material->GetTexture(type, 0, &texturePath);
            fullTexturePath = fullTexturePath.append(texturePath.C_Str());

            if (textureAtlas.count(fullTexturePath) == 0){
                Texture texture;
                // bool srgb = type == aiTextureType_NORMALS || type == aiTextureType_HEIGHT;
                bool srgb = false;
                texture.loadTexture(fullTexturePath, srgb);
                textureAtlas.insert({fullTexturePath, texture});
            }

            textures.push_back(textureAtlas.at(fullTexturePath).textureID);
        }
        else{

            if(type == aiTextureType_LIGHTMAP){
                textures.push_back(0);
            }
            if(type == aiTextureType_EMISSIVE){
                textures.push_back(0);
            }
            if(type == aiTextureType_NORMALS){
                textures.push_back(0);
            }
            if(type == aiTextureType_UNKNOWN){
                textures.push_back(0);
            }
        }
    }

    //Diffuse textures
    // type = aiTextureType_DIFFUSE;
    // fullTexturePath = directory;
    // if( material->GetTextureCount(type) > 0 ){
        //We only care about the first texture assigned we don't expect multiple to be assigned
        // material->GetTexture(type, 0, &texturePath);
        // fullTexturePath = fullTexturePath.append(texturePath.C_Str());
    // }
    // else{
        // fullTexturePath = fullTexturePath + "dummy.dds";
    // }
    //Checking if this is hte first tiem we are loading this texture 
    // if (textureAtlas.count(fullTexturePath) == 0){
    //     Texture texture;
    //     // texture.type = "diffuse";
    //     texture.setupTexture(fullTexturePath, false);
    //     textureAtlas.insert({fullTexturePath, texture});
    // }

    // //No matter what, you push it to the vector of texures id's
    // textures.push_back(textureAtlas.at(fullTexturePath).textureID);

    // //Roughness textures 
    // type = aiTextureType_SPECULAR;
    // fullTexturePath = directory;
    // if( material->GetTextureCount(type) > 0 ){
    //     //We only care about the first texture assigned we don't expect multiple to be assigned
    //     material->GetTexture(type, 0, &texturePath);
    //     fullTexturePath = fullTexturePath.append(texturePath.C_Str());
    // }
    // else{
    //     fullTexturePath = fullTexturePath + "dummy_specular.dds";
    // }
    // //Checking if this is hte first tiem we are loading this texture 
    // if (textureAtlas.count(fullTexturePath) == 0){
    //     Texture texture;
    //     texture.type = "roughness";
    //     texture.setupTexture(fullTexturePath, false);
    //     textureAtlas.insert({fullTexturePath, texture});
    // }

    // //No matter what, you push it to the vector of texures id's
    // textures.push_back(textureAtlas.at(fullTexturePath).textureID);


    // //TODO: Make this either height or normals depending on the model
    // type = aiTextureType_HEIGHT;
    // // type = aiTextureType_NORMALS;

    // fullTexturePath = directory;
    // if( material->GetTextureCount(type) > 0 ){
    //     //We only care about the first texture assigned we don't expect multiple to be assigned
    //     material->GetTexture(type, 0, &texturePath);
    //     fullTexturePath = fullTexturePath.append(texturePath.C_Str());
    // }
    // else{
    //     fullTexturePath = fullTexturePath + "dummy_ddn.dds";
    // }
    // //Checking if this is hte first tiem we are loading this texture 
    // if (textureAtlas.count(fullTexturePath) == 0){
    //     Texture texture;
    //     texture.type = "normal";
    //     texture.setupTexture(fullTexturePath, false);
    //     textureAtlas.insert({fullTexturePath, texture});
    // }

    // //No matter what, you push it to the vector of texures id's
    // textures.push_back(textureAtlas.at(fullTexturePath).textureID);

    // ///--------------------------------------------------------------------------------------
    // // New pbr stuff

    // //Metallic texture 
    // type = aiTextureType_AMBIENT;
    // fullTexturePath = directory;
    // if( material->GetTextureCount(type) > 0 ){
    //     //We only care about the first texture assigned we don't expect multiple to be assigned
    //     material->GetTexture(type, 0, &texturePath);
    //     fullTexturePath = fullTexturePath.append(texturePath.C_Str());
    // }
    // else{
    //     fullTexturePath = fullTexturePath + "dummy.dds";
    // }
    // //Checking if this is hte first tiem we are loading this texture 
    // if (textureAtlas.count(fullTexturePath) == 0){
    //     Texture texture;
    //     texture.type = "metallic";
    //     texture.setupTexture(fullTexturePath, false);
    //     textureAtlas.insert({fullTexturePath, texture});
    // }

    // textures.push_back(textureAtlas.at(fullTexturePath).textureID);

    return textures;
}