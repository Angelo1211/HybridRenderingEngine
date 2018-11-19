#ifndef GEOMETRY_H
#define GEOMETRY_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-10
PURPOSE      : Contains all basic geometric objects used in the engine for rendering and culling. 
SPECIAL NOTES: Currently view Frustum culling is disabled since it would require a rewrite of the 
               model/mesh system. This is a high priority fix.
*/

//Headers
#include "glm/glm.hpp"
#include "mesh.h"

//Struct containing vertex data for a AABB   
//around the model. Primarily for use in frustum culling
//Regiong R = {(x, y, z) | min.x <=x <= max.x | same Y | same Z}
struct AABox{
    glm::vec3 minPoints;
    glm::vec3 maxPoints;

    //Builds axis aligned bounding box of the given mesh
    void buildAABB(const Mesh &mesh);
    void update(const glm::mat4 &modelMatrix);
};

//Only used in frustrum culling, a frustrum has 6 planes
//Equation is Ax + By + Cz + D = 0 
struct Plane{
    glm::vec3 normal;
    float D;

    float distance(const glm::vec3 &points);
    void setNormalAndPoint(const glm::vec3 &normal, const glm::vec3 &point);
};

//The shape of the camera view area, looks like an inverse pyramid with the top missing
struct Frustum{
    enum planes
    {
        TOP = 0,
        BOTTOM,
        LEFT,
        RIGHT,
        NEARP,
        FARP
    };

    Plane pl[6];
    float fov, nearPlane, farPlane, AR, nearH, nearW;

    void setCamInternals();
    void updatePlanes(glm::mat4 &viewMat, const glm::vec3 &cameraPos);
    bool checkIfInside(AABox *bounds);
};
#endif