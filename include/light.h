#ifndef LIGHT_H
#define LIGHT_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-20
PURPOSE      : 
SPECIAL NOTES: 
*/

#include "glm/glm.hpp"

struct BaseLight{
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 ambient = glm::vec3(0.02f);
    glm::vec3 specular = glm::vec3(0.5f);
    glm::vec3 diffuse = glm::vec3(0.5f);

    glm::mat4 shadowProjectionMat = glm::mat4(0.0);

    bool  changed = false;

    float strength = 1.0f;
    float zNear    = 1.0f;
    float zFar     = 2000.0f;

    unsigned int shadowRes = 1024;
    unsigned int depthMapTextureID = 0;
};

struct DirectionalLight : public BaseLight{
    glm::vec3 direction = glm::vec3(-1.0f);

    glm::mat4 lightView = glm::mat4(0.0);
    glm::mat4 lightSpaceMatrix = glm::mat4(0.0);

    float distance = 0.0f;
    float orthoBoxSize = 3000.0f; 
};

struct PointLight : public BaseLight{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 attenuation = glm::vec3(1.0f, 0.0f, 0.0f);

    const float ang = glm::radians(90.0f);
    static const unsigned int numFaces = 6;

    float aspect   = 1.0f;

    glm::mat4 lookAtPerFace[numFaces];
};

struct GPULight{
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};

#endif