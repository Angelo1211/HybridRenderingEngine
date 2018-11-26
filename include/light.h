#ifndef LIGHT_H
#define LIGHT_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-20
PURPOSE      : Data structures for the different types of light. Both CPU and GPU versions. 
SPECIAL NOTES: All light data struct has to be re-written in a more GPU friendly way asap during the shadow mapping 
               rewrite. Ideally, we'd have only one struct that is valid for all data types so that we can
               deal with all lights equally in the shadow mapping and rendering process.
*/

#include "glm/glm.hpp"

struct BaseLight{
    glm::vec3 color = glm::vec3(1.0f);
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

    float distance;
    float orthoBoxSize; 
};
struct PointLight : public BaseLight{
    glm::vec3 position = glm::vec3(0.0f);
    glm::mat4 lookAtPerFace[6];
};

//Currently only used in the generation of SSBO's for light culling and rendering
//I think it potentially would be a good idea to just have one overall light struct for all light types
//and move all light related calculations to the gpu via compute or frag shaders. This should reduce the
//number of Api calls we're currently making and also unify the current lighting path that is split between 
//compute shaders and application based calculations for the matrices.
struct GPULight{
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};

#endif