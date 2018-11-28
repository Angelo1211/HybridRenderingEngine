#ifndef CAMERA_H
#define CAMERA_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-11
PURPOSE      : Managing and keeping track of a camera's properties and state. 
SPECIAL NOTES: Currently a simple free-flying camera with no collision detection. There should be
               more types such as FPS for testing. Low priority feature. No fov changes implemented yet
               since it requires a cluster rebuild from scratch. Low priority feature.
*/

//Headers
#include "geometry.h"
#include "displayManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <set>

struct Camera{
    Camera(glm::vec3 tar, glm::vec3 pos, float fov,
           float speed, float sens, float nearP, float farP)
    {
        //Position and orientation of the camera, both in cartesian and spherical
        position = pos;
        target   = tar;
        front = glm::normalize(target - position);
        side  = glm::normalize(glm::cross(front, up));
        pitch = getPitch(front);
        yaw   = getYaw(front, pitch);

        //Saving reset position values
        originalPosition = pos;
        originalTarget   = tar;
        originalFront    = front;
        originalSide     = side;
        originalPitch    = pitch;
        originalYaw      = yaw;

        //Shaping the frustum to the scene's imported values
        cameraFrustum.fov = fov;
        cameraFrustum.AR  = DisplayManager::SCREEN_ASPECT_RATIO;
        cameraFrustum.farPlane  = farP;
        cameraFrustum.nearPlane = nearP;

        //Setting default values of other miscellaneous camera parameters
        camSpeed   = speed;
        mouseSens  = sens;
        blurAmount = 0;
        exposure   = 1.0f;

        //Setting up perspective and view matrix for rendering
        viewMatrix = glm::lookAt(position, target, up);
        projectionMatrix = glm::perspective(glm::radians(cameraFrustum.fov),
                                            cameraFrustum.AR, cameraFrustum.nearPlane, 
                                            cameraFrustum.farPlane);
        cameraFrustum.updatePlanes(viewMatrix, position);
    }

    //Updates the camera matrices with the user input 
    void update(unsigned int deltaT);
    void resetCamera(); 

    //Cartesian to spherical coordinates 
    float getPitch(glm::vec3 front);
    float getYaw(glm::vec3 front, float pitch);
    void updateOrientation();
    
    //View frustrum culling
    bool checkVisibility(AABox *bounds);

    //Containers for the camera transform matrices and frustum geometry 
    glm::mat4 viewMatrix, projectionMatrix;
    Frustum cameraFrustum;

    //Keeps track of the current relevant keys that are pressed to avoid issues with 
    //the frequency of polling of the keyboard vs the frequency of updates 
    std::set<char> activeMoveStates;

    //Original values used to initialize the camera
    //We keep them in memory in case user wants to reset position
    glm::vec3 originalPosition, originalTarget, originalFront, originalSide;
    float originalPitch, originalYaw;

    //Camera basis vectors for view matrix construction 
    glm::vec3 position, side, front, target, up{0,1,0};
    float pitch, yaw;

    //Physical/Optical properties
    float camSpeed, mouseSens, exposure;
    int blurAmount;
};

#endif