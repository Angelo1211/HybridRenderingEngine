// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-10
// ===============================

//Includes
#include "mesh.h"
#include <string>

void Mesh::draw(const Shader &shader, const tAtlas &textureAtlas, bool textured){
    //TODO: texture managing 
    unsigned int nDiffuse  = 0;
    unsigned int nSpecular = 0;
    if(textured){
        // if(textures.size() > 0){
        //     //Diffuse
        //     glActiveTexture(GL_TEXTURE0);
        //     shader.setInt("diffuse1", 1);
        //     glBindTexture(GL_TEXTURE_2D, textureAtlas.at(textures[0]).textureID);

        //     if( textures.size() > 1){
        //         // Specular
        //         glActiveTexture(GL_TEXTURE1);
        //         shader.setInt("specular1", 1);
        //         glBindTexture(GL_TEXTURE_2D, textureAtlas.at(textures[1]).textureID);
        //     }
        // }

        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            //Activate next texture unit
            glActiveTexture(GL_TEXTURE0 + i);
            currentTex = textureAtlas.at(textures[i]);

            //Check the type of hte texture to increment counter accordingly
            std::string name = currentTex.type;
            std::string number;
            if (name == "diffuse")
            {
                ++nDiffuse;
                number = std::to_string(nDiffuse);
            }
            else if (name == "specular")
            {
                ++nSpecular;
                number = std::to_string(nSpecular);
            }
            shader.setInt((name + number).c_str(), i);

            //Actually binding the texture now
            glBindTexture(GL_TEXTURE_2D, currentTex.textureID);
        }
        glActiveTexture(GL_TEXTURE0);
    }

    //Mesh Drawing
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh(){
    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO stuff 
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    //EBO stuff
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    //Vertex position pointer init
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    //Vertex normal pointer init
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    //Vertex texture coord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    //Unbinding VAO
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
//Quad Class stuff
void Quad::setupQuad(){
    const float quadVertices[] = {
        //positions //texCoordinates
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    //OpenGL postprocessing quad setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    //Quad position pointer initialization in attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    //Quad texcoords pointer initialization in attribute array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Quad::draw(const unsigned int readTexture1, const unsigned int readTexture2){
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);

    //A texture must always be defined (for now)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, readTexture1);

    //A texture id of 0 is never assigned by opengl so we can
    //be sure that it means we haven't set any texture in the second paramenter and therefore
    //we only want one texture
    if(readTexture2 != 0){
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, readTexture2);
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}