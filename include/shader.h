#ifndef SHADER_H
#define SHADER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-08
PURPOSE      : Load parse and compile opengl shader programs. Also can set their uniforms and indicate
               that they will be used.
SPECIAL NOTES: Error checking is fine for now but should probably send the info upstream to quit early 
               if something comes up, or at least print to the console. Low priority.
*/

//Includes
#include <glad/glad.h>
#include <string>
#include "glm/glm.hpp"

struct Shader{
    //Shader ID for referencing
    unsigned int ID;

    //Constructor using vertex, geometry and fragment shader paths
    Shader(){};
    Shader(const std::string vertexPath, const std::string fragmentPath,
           const std::string geometryPath = "");

    //Activate shader for use
    void use();

    //Utility functions
    void setBool(const std::string &name, bool value) const; 
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;

};

//Compute shaders require a different constructor since they are simpler and stored elsewhere
struct ComputeShader : public Shader{
    ComputeShader(const std::string computePath);
    void dispatch(unsigned int x, unsigned int y = 1, unsigned int z = 1);
};

#endif