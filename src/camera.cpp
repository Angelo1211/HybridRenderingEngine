/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-11 
*/

//Headers
#include "camera.h"
#include "SDL.h"

//Updates the cameras orientation and position based on the input from the user
///Also updates view matrix and projection matrix for rendering
void Camera::update(unsigned int deltaT){
    float speed = camSpeed * deltaT;

    //We apply the rotation first
    updateOrientation();

    //Then translate
    for(char x : activeMoveStates){
        switch (x){
            case 'w':
                position += front * speed;
            break;

            case 's':
                position -= front * speed;
            break;

            case 'a':
                position -= side * speed;
            break;

            case 'd':
                position += side * speed;
            break;

            case 'q':
                position += up * speed;
            break;

            case 'e':
                position -= up * speed;
            break;
        }
    }

    //And we recalculate the new view and projection matrices for rendering
    target = position + front;
    viewMatrix = glm::lookAt(position, target, up);
    cameraFrustum.updatePlanes(viewMatrix, position);
    projectionMatrix = glm::perspective(glm::radians(cameraFrustum.fov), cameraFrustum.AR, cameraFrustum.nearPlane, cameraFrustum.farPlane);
}

//View frustrum culling using a models AABB
bool Camera::checkVisibility(AABox *bounds){
    return cameraFrustum.checkIfInside(bounds);
}

//Used by input to reset camera to origin in case user loses their bearings
void Camera::resetCamera(){
    position = originalPosition;
    target   = originalTarget;
    front    = originalFront;
    side     = originalSide;
    pitch    = originalPitch;
    yaw      = originalYaw;
}

//Transform from cartesian to spherical, used in the first setup of yaw and pitch
//Since the incoming target and position values are being read from an unknown scene file
float Camera::getPitch(glm::vec3 front){
    return glm::degrees(glm::asin(front.y));
}
float Camera::getYaw(glm::vec3 front, float pitch){
    return glm::degrees(glm::acos(front.x / cos(glm::radians(pitch))));
}

//Orient the front and side vectors based on the screen pitch and yaw values
void Camera::updateOrientation(){
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front = glm::normalize(front);
    side  = glm::normalize(glm::cross(front, up));
}