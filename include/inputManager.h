#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-11
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Headers
// #include "sceneManager.h"
#include "camera.h"
#include "SDL.h"

class InputManager{
    public:
        //Dummy constructors / Destructors
        //Not really necessary here, but follows the same format for consistency
        InputManager();
        ~InputManager();

        bool startUp();
        void shutDown();

        //Processes all the SDL events that have ocurred in the past frame
        void processInput(bool &done, unsigned int deltaT);
        void setCamera(Camera *testCamera);
    private:
        // SceneManager *sceneController;
        Camera *sceneCamera;
        
        //Where specific events are handled
        void handleEvent(SDL_Event * event, bool &done, unsigned int deltaT);
};


#endif