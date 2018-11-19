/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-11
*/

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

//Simply fetches the current scene's camera
bool InputManager::startUp(SceneManager sceneManager){
    sceneController = &sceneManager;
    sceneCamera = (sceneController->getCurrentScene()->getCurrentCamera());

    return true;
}

//Does nothing yet
void InputManager::shutDown(){}

/*
While there are any remaining events on the SDL event stack
1. Check if one of those events is a quit
2. Check if the GUI wants to use the keyboard and mouse
3. Handle keyboard events ourselves
4. Handle mouse events directly 
*/
void InputManager::processInput(bool &done, unsigned int deltaT){
    SDL_Event event;

    ImGuiIO& io = ImGui::GetIO();

    while(SDL_PollEvent(&event)){
        //First check if user requests an exit
        if(event.type == SDL_QUIT){
            done = true;
            return;       
        }
        //Next, check if imGUI wants to use the mouse or keyboard
        else if(io.WantCaptureKeyboard || io.WantCaptureMouse  ){
            //Stops all camera movement if you are interacting with the GUI
            sceneCamera->activeMoveStates.clear();
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        //Handle any other relevant input data 
        //Keypresses, mouse etc
        else{
            handleEvent(&event, done, deltaT);
        }
    }
}


//Maybe a candidate to break apart into smaller functions
void InputManager::handleEvent(SDL_Event * event, bool &done, unsigned int deltaT){

    //For keyboard input we want to avoid repeated movement when the key is held
    //Instead of actually updating the camera position for each key call we update a 
    //container that keeps track of which move state the camera is in. This state is only
    //changed on keydown or key up events, freeing it from the keyboard polling rate dependency.
    bool isDown  = event->type == SDL_KEYDOWN;
    bool wasDown = event->type == SDL_KEYUP;

    if(isDown || wasDown){
        switch (event->key.keysym.sym){
            case SDLK_ESCAPE:
                if (isDown)
                    done = true;
                return;

            case SDLK_r:
                if (isDown)
                    sceneCamera->resetCamera();
                break;

            case SDLK_w:
                if (isDown)
                    sceneCamera->activeMoveStates.insert('w');
                if (wasDown)
                    sceneCamera->activeMoveStates.erase('w');
                break;

            case SDLK_s:
                if (isDown)
                    sceneCamera->activeMoveStates.insert('s');
                if (wasDown)
                    sceneCamera->activeMoveStates.erase('s');
                break;

            case SDLK_a:
                if (isDown)
                    sceneCamera->activeMoveStates.insert('a');
                if (wasDown)
                    sceneCamera->activeMoveStates.erase('a');
                break;

            case SDLK_d:
                if (isDown)
                    sceneCamera->activeMoveStates.insert('d');
                if (wasDown)
                    sceneCamera->activeMoveStates.erase('d');
                break;

            case SDLK_q:
                if (isDown)
                    sceneCamera->activeMoveStates.insert('q');
                if (wasDown)
                    sceneCamera->activeMoveStates.erase('q');
                break;

            case SDLK_e:
                if (isDown)
                    sceneCamera->activeMoveStates.insert('e');
                if (wasDown)
                    sceneCamera->activeMoveStates.erase('e');
                break;

            default:
                break;
        }
    }
    // Handling Mouse Motison
    else if (event->type == SDL_MOUSEMOTION )
    {
        //Only move camera if the left button is pressed
        if (event->motion.state & SDL_BUTTON_LMASK)
        {
            //While left button is pressed change mouse to relative mode
            SDL_SetRelativeMouseMode(SDL_TRUE);

            float sens = sceneCamera->mouseSens;
            float xOffset = (float)event->motion.xrel * sens; 
            float yOffset = -(float)event->motion.yrel * sens;

            //To reduce precision issues we keep the yaw constrained to 360 degrees
            sceneCamera->yaw   = glm::mod(sceneCamera->yaw + xOffset, 360.0f);
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
        }
        else{
            //Once the left butto is not pressed set the mouse to normal mode
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }
}