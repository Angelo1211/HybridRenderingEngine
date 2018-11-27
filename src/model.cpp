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

//We use assimp to load all our mesh files this 
void Model::loadModel(std::string path){
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes |aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

    //useful for texture indexing later
    fileExtension = FLOAD::getFileExtension(path);
    directory = path.substr(0, path.find_last_of('/'));
    directory += "/";

    //begin recursive processing of loaded model
    processNode(scene->mRootNode, scene);
}

//The model currently is just a vessel for the meshes of the scene,
//In a future revision this will probably change
void Model::draw(const Shader &shader, const  bool textured){
    shader.setBool("IBL", IBL);
    for(int i = 0; i < meshes.size(); ++i){
        meshes[i].draw(shader, textured);
    }
}

//Stub, to fill in later
void Model::update(const unsigned int deltaT){
}

//Basic ASSIMP scene tree traversal, taken from the docs
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

/*
1. Process vertices 
2. Process indices
3. Process materials

TODO::Refactoring target?
*/
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int > indices;
    std::vector<unsigned int > textures;

    //Process vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; ++i){
        //Process vertex positions, normals, tangents, bitangents, and texture coordinates
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
        if (mesh->HasTextureCoords(0)){
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
FIXES::
1. Have more than one texture per type
2. Make this it's own material class that takes care of it properly
*/
std::vector<unsigned int> Model::processTextures(const aiMaterial *material){
    std::vector<unsigned int> textures;

    //Finding current texture directory
    aiString texturePath;
    aiTextureType type;
    std::string fullTexturePath;

    //Checking all texture stacks for each texture type
    //Checkout assimp docs on texture types
    for(int tex = aiTextureType_NONE ; tex <= aiTextureType_UNKNOWN; tex++){
        type = static_cast<aiTextureType>(tex); //making the int value into the enum value
        fullTexturePath = directory;

        //If there are any textures of the given type in the material
        if( material->GetTextureCount(type) > 0 ){
            //We only care about the first texture assigned we don't expect multiple to be assigned
            material->GetTexture(type, 0, &texturePath);
            fullTexturePath = fullTexturePath.append(texturePath.C_Str());

            //If this texture has not been added to the atlas yet we load it
            if (textureAtlas.count(fullTexturePath) == 0){
                Texture texture;
                bool srgb = false;
                texture.loadTexture(fullTexturePath, srgb);
                textureAtlas.insert({fullTexturePath, texture});
            }

            //We add it to the texture index array of loaded texture for a given mesh
            textures.push_back(textureAtlas.at(fullTexturePath).textureID);
        }
        else{
            //For now we always assume that these textures will exist in the current
            //material. If they do not, we assign 0 to their value.
            //This will be fixed when the new material model is implemented.
            switch(type){
                case aiTextureType_LIGHTMAP:
                case aiTextureType_EMISSIVE:
                case aiTextureType_NORMALS:
                case aiTextureType_UNKNOWN:
                    textures.push_back(0);
                break;
            }
        }
    }
    return textures;
}