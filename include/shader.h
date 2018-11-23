#ifndef SHADER_H
#define SHADER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-08
PURPOSE      : Load parse and compile opengl shader programs. Also can set their uniforms and indicate
               that they will be used.
SPECIAL NOTES: We have to minimize shader passing, currently each pass can potentially cause a shader deletion.
               Need to come up with a robust solution to how to avoid deletion. Priority high
*/

//Includes
#include <glad/glad.h>
#include <string>
#include "glm/glm.hpp"

struct Shader{
    //Initialization 
    bool setup(const std::string vertexPath, const std::string fragmentPath,
           const std::string geometryPath = "");

    //Shader program marked for deletion on destructor call
    //CAREFUL with passing by value, any destructor call wipes the shader out!!
    ~Shader(){
        glDeleteProgram(ID);
    }

    //Activate shader program for use
    void use() const;

    //Uniform setting functions
    void setBool(const std::string &name, bool value) const; 
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;

    //Shader program ID for referencing
    unsigned int ID;
};

//Compute shaders require a different constructor since they are simpler and stored elsewhere
struct ComputeShader : public Shader{
    bool setup(const std::string computePath);
    void dispatch(unsigned int x, unsigned int y = 1, unsigned int z = 1) const;
};

#endif