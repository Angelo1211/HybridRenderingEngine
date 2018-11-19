#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-11
PURPOSE      : Read and process user input for the given frame and distinguish between
               input meant for the GUI and input meant for the camera controls.
SPECIAL NOTES: Currently, mouse wheel as a way of change field of view is disabled since
               changes in the field of view require a rebuilding of the view frustum 
               clusters. It is a low priority feature. 
*/

//Headers
#include "sceneManager.h"
#include "camera.h"
#include "SDL.h"

class InputManager{
    public:
        //Dummy constructors / Destructors
        //Not really necessary here, but follows the same format for consistency
        InputManager();
        ~InputManager();

        bool startUp(SceneManager sceneManager);
        void shutDown();

        //Processes all the SDL events that have ocurred in the past frame
        void processInput(bool &done, unsigned int deltaT);

    private:
        SceneManager *sceneController;
        Camera *sceneCamera;

        //Handle all events in one convenient function
        void handleEvent(SDL_Event * event, bool &done, unsigned int deltaT);
};


#endif