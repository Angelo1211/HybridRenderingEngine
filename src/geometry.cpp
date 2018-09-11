// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-10
// ===============================

#include "geometry.h"
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <algorithm>

//-------------------------------AABOX------------------------------------//

void AABox::buildAABB(const Mesh &mesh){
    glm::vec3 minVals(std::numeric_limits<float>::max());
    glm::vec3 maxVals(std::numeric_limits<float>::min());

    //Iterating through every vertx in the mesh
    for(int i = 0; i < mesh.vertices.size(); ++i){
        //Setting max values
        maxVals.x = std::max(maxVals.x, mesh.vertices[i].position.x);
        maxVals.y = std::max(maxVals.y, mesh.vertices[i].position.y);
        maxVals.z = std::max(maxVals.z, mesh.vertices[i].position.z);

        //Setting min values
        minVals.x = std::min(minVals.x, mesh.vertices[i].position.x);
        minVals.y = std::min(minVals.y, mesh.vertices[i].position.y);
        minVals.z = std::min(minVals.z, mesh.vertices[i].position.z);
    }
    minPoints = minVals;
    maxPoints = maxVals;
}
//TODO TODO TODO
//Update AABB from rotated AABB 
void AABox::update(const glm::mat4 &modelMatrix){
    glm::vec3 minVals(std::numeric_limits<float>::max());
    glm::vec3 maxVals(std::numeric_limits<float>::min());
    glm::vec3 vertices[8];

    // //Reconstructing the 8 vertices of an AABB from the min and max valuesin the struct
    // vertices[0] = glm::vec3(minPoints.x, minPoints.y, minPoints.z);
    // vertices[1] = glm::vec3(maxPoints.x, minPoints.y, minPoints.z);
    // vertices[2] = glm::vec3(minPoints.x, maxPoints.y, minPoints.z);
    // vertices[3] = glm::vec3(maxPoints.x, maxPoints.y, minPoints.z);
    // vertices[4] = glm::vec3(minPoints.x, minPoints.y, maxPoints.z);
    // vertices[5] = glm::vec3(maxPoints.x, minPoints.y, maxPoints.z);
    // vertices[6] = glm::vec3(minPoints.x, maxPoints.y, maxPoints.z);
    // vertices[7] = glm::vec3(maxPoints.x, maxPoints.y, maxPoints.z);

    // //Iterating through every vertx in the mesh
    // for(int i = 0; i < 8; ++i){
    //     //Checking the current vertex for all axes
    //     for(int ax = 0; ax < 3; ++ax){
    //         //Setting max values
    //         if(vertices[i].data[ax] > maxVals.data[ax]){
    //             maxVals.data[ax] = vertices[i].data[ax];
    //         }
    //         //Setting min values
    //         if(vertices[i].data[ax] < minVals.data[ax]){
    //             minVals.data[ax] = vertices[i].data[ax];
    //         }
    //     }
    // }

    // minPoints = minVals;
    // maxPoints = maxVals;
}


//---------------------------------PLANE------------------------------------//
//Returns a negative value if not aligned in the same direction of plane normal
//which I established to be pointing towards the interior of the camera frustrum
float Plane::distance(const glm::vec3 &points){
    return glm::dot(normal, points) + D;
}

void Plane::setNormalAndPoint(const glm::vec3 &n, const glm::vec3 &p0){
    normal = n;
    D = -glm::dot(n, p0);
}

//-------------------------------FRUSTRUM------------------------------------//

void Frustrum::setCamInternals(){
    float tanHalfFOV  =  tan( (fov/2) * (M_PI / 180) );
    nearH = nearPlane * tanHalfFOV; //Half of the frustrum near plane height
    nearW = nearH * AR;
}

//Calculates frustrum planes in world space
void Frustrum::updatePlanes(glm::mat4 &viewMat, const glm::vec3 &cameraPos){
    setCamInternals();
    glm::vec3 X(viewMat[0][0], viewMat[0][1], viewMat[0][2]); //Side Unit Vector
    glm::vec3 Y(viewMat[1][0], viewMat[1][1], viewMat[1][2]); //Up Unit Vector
    glm::vec3 Z(viewMat[2][0], viewMat[2][1], viewMat[2][2]); //Forward vector
    
    //Gets worlds space position of the center points of the near and far planes
    //The forward vector Z points towards the viewer so you need to negate it and scale it
    //by the distance (near or far) to the plane to get the center positions
    glm::vec3 nearCenter = cameraPos - Z * nearPlane;
    glm::vec3 farCenter  = cameraPos - Z * farPlane;

    glm::vec3 point;
    glm::vec3 normal;

    //We build the planes using a normal and a point (in this case the center)
    //Z is negative here because we want the normal vectors we choose to point towards
    //the inside of the view frustrum that way we can cehck in or out with a simple 
    //Dot product
    pl[NEARP].setNormalAndPoint(-Z, nearCenter);
    //Far plane 
    pl[FARP].setNormalAndPoint(Z, farCenter);

    //Again, want to get the plane from a normal and point
    //You scale the up vector by the near plane height and added to the nearcenter to 
    //optain a point on the edge of both near and top plane.
    //Subtracting the cameraposition from this point generates a vector that goes along the 
    //surface of the plane, if you take the cross product with the direction vector equal
    //to the shared edge of the planes you get the normal
    point  = nearCenter + Y*nearH;
    normal = glm::normalize(point - cameraPos);
    normal = glm::cross(normal, X);
    pl[TOP].setNormalAndPoint(normal, point);

    //Bottom plane
    point  = nearCenter - Y*nearH;
    normal = glm::normalize(point - cameraPos);
    normal = glm::cross(X, normal);
    pl[BOTTOM].setNormalAndPoint(normal, point);

    //Left plane
    point  = nearCenter - X*nearW;
    normal = glm::normalize(point - cameraPos);
    normal = glm::cross(normal, Y);
    pl[LEFT].setNormalAndPoint(normal, point);

    //Right plane
    point  = nearCenter + X*nearW;
    normal = glm::normalize(point - cameraPos);
    normal = glm::cross(Y, normal);
    pl[RIGHT].setNormalAndPoint(normal, point);
    
}

//False is fully outside, true if inside or intersects
//based on iquilez method
bool Frustrum::checkIfInside(AABox *box){

    //Check box outside or inside of frustrum
    for(int i =0; i < 6; ++i){
        int out = 0;
        out += ((pl[i].distance(glm::vec3(box->minPoints.x, box->minPoints.y, box->minPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->maxPoints.x, box->minPoints.y, box->minPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->minPoints.x, box->maxPoints.y, box->minPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->maxPoints.x, box->maxPoints.y, box->minPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->minPoints.x, box->minPoints.y, box->maxPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->maxPoints.x, box->minPoints.y, box->maxPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->minPoints.x, box->maxPoints.y, box->maxPoints.z))< 0.0 )?1:0);
        out += ((pl[i].distance(glm::vec3(box->maxPoints.x, box->maxPoints.y, box->maxPoints.z))< 0.0 )?1:0);
        
        if (out == 8) return false;
    }
    return true;
}