#ifndef GEOMETRY_H
#define GEOMETRY_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-10
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

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
//Equation is Ax + By + Cz + D = 0 (or somthing)
struct Plane{
    glm::vec3 normal;
    float D;

    float distance(const glm::vec3 &points);
    void setNormalAndPoint(const glm::vec3 &normal, const glm::vec3 &point);
};

//The shape of the camera view area
class Frustrum{
    private:
    	enum planes{
		TOP = 0, BOTTOM, LEFT,
		RIGHT, NEARP, FARP};
    public:
        float fov, nearPlane, farPlane, AR, nearH, nearW;
        Plane pl[6];
        Frustrum(float ratio)
        : fov(50.0f),
          nearPlane(1.0f),
          farPlane(7000.0f),
          AR(ratio) 
        { }

        void setCamInternals();
        void updatePlanes(glm::mat4 &viewMat, const glm::vec3 &cameraPos) ;
        bool checkIfInside(AABox *bounds);
};
#endif