// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-11
// ===============================

//Headers
#include "inputManager.h"
#include <string>
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

//Dummy constructors and destructors
InputManager::InputManager(){}
InputManager::~InputManager(){}

bool InputManager::startUp(SceneManager sceneManager){
    sceneController = &sceneManager;
    sceneCamera = (sceneController->getCurrentScene()->getCurrentCamera());
    
    //Only really care about relative mouse motion because we're building a free camera
    // bool success = !SDL_SetRelativeMouseMode(SDL_TRUE);
    // return success;
    return true;
}

void InputManager::shutDown(){
    //Nothing to do yet
}

//Goes through the list of every event that has occurred since the last call
//of this function and either performs and exit or sends the result to the even handler
void InputManager::processInput(bool &done, unsigned int deltaT){
    SDL_Event event;
    ImGuiIO& io = ImGui::GetIO();
    while(SDL_PollEvent(&event)){
        //First check if user requests an exit
        if(event.type == SDL_QUIT){
            done = true;
            return;       
        }
        else if(io.WantCaptureKeyboard || io.WantCaptureMouse  ){
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        //Handle any other relevant input data 
        //Keypresses, mouse etc
        else{
            handleEvent(&event, done, deltaT);
        }
    }
}
// TODO IMPORTANT REFACTOR THIS UGLY FUNCTION
void InputManager::handleEvent(SDL_Event * event, bool &done, unsigned int deltaT){
    bool isDown  = event->type == SDL_KEYDOWN;
    bool wasDown = event->type == SDL_KEYUP;

    switch (event->key.keysym.sym){
        case SDLK_ESCAPE:
            if (isDown)
                done = true;
            return;
        
        case SDLK_w:
            if (isDown)
                sceneCamera->set.insert('w');
            if (wasDown)
                sceneCamera->set.erase('w');
            break;

        case SDLK_s:
            if (isDown)
                sceneCamera->set.insert('s');
            if (wasDown)
                sceneCamera->set.erase('s');
            break;

        case SDLK_a:
            if (isDown)
                sceneCamera->set.insert('a');
            if (wasDown)
                sceneCamera->set.erase('a');
            break;

        case SDLK_d:
            if (isDown)
                sceneCamera->set.insert('d');
            if (wasDown)
                sceneCamera->set.erase('d');
            break;

        case SDLK_q:
            if (isDown)
                sceneCamera->set.insert('q');
            if (wasDown)
                sceneCamera->set.erase('q');
            break;

        case SDLK_e:
            if (isDown)
                sceneCamera->set.insert('e');
            if (wasDown)
                sceneCamera->set.erase('e');
            break;

        default:
            break;
    }
    // Handling Mouse Motison
    if (event->type == SDL_MOUSEMOTION)
    {
        //Only move camera if the right button is pressed
        if (event->motion.state & SDL_BUTTON_LMASK)
        {
            float sens = 0.05f;
            float xOffset = (float)event->motion.xrel * sens;
            float yOffset = -(float)event->motion.yrel * sens;

            sceneCamera->yaw += xOffset;
            sceneCamera->pitch += yOffset;

            //Limiting the range of the pitch to avoid flips
            if (sceneCamera->pitch > 89.0f)
            {
                sceneCamera->pitch = 89.0f;
            }
            else if (sceneCamera->pitch < -89.0f)
            {
                sceneCamera->pitch = -89.0f;
            }

            //Updating the front and side vectors to allow wasd movement and
            //free camera movement.
            sceneCamera->front.x = cos(glm::radians(sceneCamera->pitch)) * cos(glm::radians(sceneCamera->yaw));
            sceneCamera->front.y = sin(glm::radians(sceneCamera->pitch));
            sceneCamera->front.z = cos(glm::radians(sceneCamera->pitch)) * sin(glm::radians(sceneCamera->yaw));
            sceneCamera->front = glm::normalize(sceneCamera->front);
            sceneCamera->side = glm::cross(sceneCamera->front, sceneCamera->up);
        }
    }
    // Handling mouse wheel movement
    // Changes zoom levels in increments of 5 degrees (2.5 really cause FOV is half angle)
    // if (event->type == SDL_MOUSEWHEEL)
    // {

    //     float zoom = 5.0f;
    //     float fov = sceneCamera->cameraFrustrum.fov;
    //     if (event->wheel.y > 0)
    //     { // scroll up
    //         fov -= zoom;
    //     }
    //     else if (event->wheel.y < 0)
    //     { // scroll down
    //         fov += zoom;
    //     }

    //     //Limiting the FOV range to avoid low FPS values or weird distortion
    //     if (fov < 20)
    //     {
    //         fov = 20;
    //     }
    //     else if (fov > 120)
    //     {
    //         fov = 120;
    //     }

    //     //Updating the camera frustrum
    //     sceneCamera->cameraFrustrum.fov = fov;
    // }
}
        //1. User requested quits
        //2. Keyboard presses
        //3. Mouse movement
        //4. Mouse clicks
        //5. Mouse wheel movement
        // void InputManager::handleEvent(SDL_Event * event, bool &done, unsigned int deltaT){
        //     float speed = sceneCamera->camSpeed *  deltaT;

        //     //Handling keyboard input
        //     if( event->type == SDL_KEYDOWN ){
        //         std::string sceneID = "0";
        //         switch( event->key.keysym.sym )
        //         {
        //             //SCENE CODE
        //             // case SDLK_1:
        //             // sceneID = "teapotSingle";
        //             // break;

        //             //MOVEMENT CONTROLS (STRAFING)
        //             //WINDOW CONTROL OPTIONS
        //             case SDLK_ESCAPE:
        //             done = true;
        //             return;
        //             case SDLK_w:
        //             printf("Moving Forward!\n");
        //             sceneCamera->position += sceneCamera->front * speed;
        //             break;

        //             case SDLK_s:
        //             sceneCamera->position -= sceneCamera->front * speed;
        //             break;



        //             //CAMERA CONTROLS (RESET AND ORBITING)
        //             // case SDLK_r:
        //             // sceneCamera->resetCamera();
        //             // break;

        //             // case SDLK_TAB:
        //             // sceneCamera->orbiting = !sceneCamera->orbiting;
        //             // sceneCamera->resetCamera();
        //             // break;

        //             // case SDLK_UP:
        //             // sceneCamera->period -= 2;
        //             // if (sceneCamera->period  < 4){
        //             //     sceneCamera->period = 4;
        //             // }
        //             // break;

        //             // case SDLK_DOWN:
        //             // sceneCamera->period += 2;
        //             // if (sceneCamera->period  > 60){
        //             //     sceneCamera->period = 60;
        //             // }
        //             // break;

        //             default:
        //             break;

        //         }

        //         //Only switch scene if a scene-key (1-5) was pressed
        //         //Exit if the scene could not be loaded for some reason
        //         // if ( sceneID != "0" ){
        //         //     if( !sceneController->switchScene(sceneID) ){
        //         //         printf("Failed to switch scene! Quitting.\n");
        //         //         done = true;
        //         //         return;
        //         //     }
        //         //     else{
        //         //         printf("Loaded %s Scene.\n", sceneID.c_str());
        //         //         sceneCamera = (sceneController->getCurrentScene()->getCurrentCamera());
        //         //         sceneCamera->resetCamera();
        //         // }
        //     }
        // }
