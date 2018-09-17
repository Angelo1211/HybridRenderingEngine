// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-11
// ===============================

//Headers
#include "inputManager.h"
#include <string>
#include "glm/glm.hpp"

//Dummy constructors and destructors
InputManager::InputManager(){}
InputManager::~InputManager(){}

bool InputManager::startUp(SceneManager sceneManager){
    sceneController = &sceneManager;
    sceneCamera = (sceneController->getCurrentScene()->getCurrentCamera());
    
    //Only really care about relative mouse motion because we're building a free camera
    bool success = !SDL_SetRelativeMouseMode(SDL_TRUE);
    return success;
}

void InputManager::shutDown(){
    //Nothing to do yet
}

//Goes through the list of every event that has occurred since the last call
//of this function and either performs and exit or sends the result to the even handler
void InputManager::processInput(bool &done, unsigned int deltaT){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        //First check if user requests an exit
        if(event.type == SDL_QUIT){
            done = true;
            return;       
        }
        //Handle any other relevant input data 
        //Keypresses, mouse etc
        else{
            handleEvent(&event, done, deltaT);
        }
    }
}

//Handles all the current valid user events:
//1. User requested quits
//2. Keyboard presses
//3. Mouse movement
//4. Mouse clicks
//5. Mouse wheel movement
void InputManager::handleEvent(SDL_Event * event, bool &done, unsigned int deltaT){
    //Normally would be multiplied by deltaT but caused issues with large fps 
    //differences
    float speed = sceneCamera->camSpeed *  deltaT;

    //Handling keyboard input
    if( event->type == SDL_KEYDOWN ){

        //Keys 1-6 handle scenes switching
        //Key ESC handles exit
        //Keys wasdqe handle strafing and moving up and down
        //Key r handles resetting camera
        //Key tab handles toggleing orbit mode
        std::string sceneID = "0";
        switch( event->key.keysym.sym )
        {   
            //SCENE CODE
            // case SDLK_1:
            // sceneID = "teapotSingle";
            // break;

            // case SDLK_2:
            // sceneID = "teapotMultiMaterial";
            // break;

            // case SDLK_3:
            // sceneID = "chest";
            // break;

            // case SDLK_4:
            // sceneID = "firehydrant";
            // break;

            // case SDLK_5:
            // sceneID = "cerberus";
            // break;

            // case SDLK_6:
            // sceneID = "statue";
            // break;

            // case SDLK_7:
            // sceneID = "multipleMesh";
            // break;

            //WINDOW CONTROL OPTIONS
            case SDLK_ESCAPE:
            done = true;
            return; 

            //MOVEMENT CONTROLS (STRAFING)
            case SDLK_w:
            if(sceneCamera->orbiting){
                sceneCamera->radius += sceneCamera->radius * speed;
            }
            else{
                sceneCamera->position += sceneCamera->front * speed;
            }
            break;

            case SDLK_s:
            if(sceneCamera->orbiting){
                sceneCamera->radius -= sceneCamera->radius * speed;
            }
            else{
                sceneCamera->position -= sceneCamera->front * speed;
            }
            break;

            case SDLK_a:
            sceneCamera->position -= sceneCamera->side * speed;
            break;

            case SDLK_d:
            sceneCamera->position += sceneCamera->side * speed;
            break;

            case SDLK_q:
            sceneCamera->position += sceneCamera->up * speed;
            break;

            case SDLK_e:
            sceneCamera->position -= sceneCamera->up * speed;
            break;

            //CAMERA CONTROLS (RESET AND ORBITING)
            case SDLK_r:
            sceneCamera->resetCamera(); 
            break;

            case SDLK_TAB:
            sceneCamera->orbiting = !sceneCamera->orbiting;
            sceneCamera->resetCamera();   
            break;

            case SDLK_UP:
            sceneCamera->period -= 2;
            if (sceneCamera->period  < 4){
                sceneCamera->period = 4;
            }
            break;

            case SDLK_DOWN:
            sceneCamera->period += 2;
            if (sceneCamera->period  > 60){
                sceneCamera->period = 60;
            }
            break;

            default:
            break;

        }

        //Only switch scene if a scene-key (1-5) was pressed
        //Exit if the scene could not be loaded for some reason
        // if ( sceneID != "0" ){
        //     if( !sceneController->switchScene(sceneID) ){
        //         printf("Failed to switch scene! Quitting.\n");
        //         done = true;
        //         return;
        //     }
        //     else{
        //         printf("Loaded %s Scene.\n", sceneID.c_str());
        //         sceneCamera = (sceneController->getCurrentScene()->getCurrentCamera());
        //         sceneCamera->resetCamera(); 
        //     }

        // }
    }
    //Handling Mouse Motion
    else if( event->type == SDL_MOUSEMOTION){
        //Only move camera if the right button is pressed
        if( event->motion.state & SDL_BUTTON_LMASK ) {
            float sens = 0.05f;
            float xOffset = (float)event->motion.xrel * sens;
            float yOffset = -(float)event->motion.yrel * sens;

            sceneCamera->yaw   += xOffset;
            sceneCamera->pitch += yOffset;

            //Limiting the range of the pitch to avoid flips
            if(sceneCamera->pitch > 89.0f){
                sceneCamera->pitch =  89.0f;
            }
            else if(sceneCamera->pitch < -89.0f){
                sceneCamera->pitch = -89.0f;
            }

            //Updating the front and side vectors to allow wasd movement and 
            //free camera movement.
            sceneCamera->front.x = cos( glm::radians(sceneCamera->pitch) ) * cos( glm::radians(sceneCamera->yaw) );
            sceneCamera->front.y = sin( glm::radians(sceneCamera->pitch) );
            sceneCamera->front.z = cos( glm::radians(sceneCamera->pitch) ) * sin( glm::radians(sceneCamera->yaw) );
            sceneCamera->front   = glm::normalize(sceneCamera->front);
            sceneCamera->side    = glm::cross(sceneCamera->front, sceneCamera->up);
        }
    }
    //Handling mouse wheel movement
    //Changes zoom levels in increments of 5 degrees (2.5 really cause FOV is half angle)
    else if( event->type == SDL_MOUSEWHEEL){

        float zoom = 5.0f;
        float fov  = sceneCamera->cameraFrustrum.fov; 
        if(event->wheel.y > 0){ // scroll up
            fov -= zoom;
        }
        else if(event->wheel.y < 0) {// scroll down
            fov += zoom;
        }

        //Limiting the FOV range to avoid low FPS values or weird distortion
        if(fov < 20){
            fov = 20;
        }
        else if (fov > 120){
            fov = 120;
        }

        //Updating the camera frustrum
        sceneCamera->cameraFrustrum.fov = fov;
    }
}