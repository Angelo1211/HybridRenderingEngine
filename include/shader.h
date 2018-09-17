#ifndef SHADER_H
#define SHADER_H
// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-07
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Includes
#include <glad/glad.h>
#include <string>
#include "glm/glm.hpp"

class Shader{
public:
    //Shader ID for referencing
    unsigned int ID;

    //Constructor using vertex and fragment shader paths
    Shader(const std::string vertexPath, const std::string fragmentPath);

    void use();

    //Utility functions
    void setBool(const std::string &name, bool value) const; 
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
};

#endif