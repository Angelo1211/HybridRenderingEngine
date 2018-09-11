// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-10
// ===============================

//Headers
#include "camera.h"
#include "SDL.h"

Camera::Camera(){
    side = glm::normalize(glm::cross(front, up));
    viewMatrix = glm::lookAt(position, target, up);
    projectionMatrix = glm::perspective(glm::radians(cameraFrustrum.fov), cameraFrustrum.AR, cameraFrustrum.nearPlane, cameraFrustrum.farPlane);
    cameraFrustrum.setCamInternals();
    cameraFrustrum.updatePlanes(viewMatrix, position);
}

//Updates target and position based on camera movement mode.
///Also updates view matrix and projection matrix for rendering
void Camera::update(unsigned int deltaT){
    if(orbiting){
        float ang    = 2 * M_PI * static_cast<float>(SDL_GetTicks()) / (period*1000);
        float camX   = std::sin(ang) * radius; 
        float camZ   = std::cos(ang) * radius;
        position.x   = camX;
        position.y   = camX;
        position.z   = camZ;
    }
    else{
        target = position + front;
    }
    viewMatrix = glm::lookAt(position, target, up);
    cameraFrustrum.updatePlanes(viewMatrix, position);
    projectionMatrix = glm::perspective(glm::radians(cameraFrustrum.fov), cameraFrustrum.AR, cameraFrustrum.nearPlane, cameraFrustrum.farPlane);
}

//View frustrum culling using a models AAB
bool Camera::checkVisibility(AABox *bounds){
    return cameraFrustrum.checkIfInside(bounds);
}

//Used by input to reset camera to origin in case user loses their bearings
void Camera::resetCamera(){
    position = glm::vec3(0, 0, 8.0);
    target   = glm::vec3(0, 0, 0 );
    front    = glm::vec3(0, 0, -1);
    side     = glm::cross(front, up);
    radius   =   2;
    pitch    =   0;
    yaw      = -90;
    period   =  30;
}