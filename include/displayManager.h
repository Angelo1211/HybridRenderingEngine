#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-05
PURPOSE      : Initialize and interface with the hardware abstraction layers such as SDL and OpenGL.
               ImGUI is also initialized and setup here since it enables the use of platform binding
               pre-included scripts. 
SPECIAL NOTES: The screen width and height are compile time constants for now and cannot be changed at 
               runtime. Changing this is a high priority feature. 
*/

//Includes
#include "SDL.h"

class DisplayManager{
    public:
        //The screen size is fixed and set at compile time 
        const static int SCREEN_WIDTH  = 1920;//1280; //640 1920
        const static int SCREEN_HEIGHT = 1080;//720 ; //480 1080
        constexpr static float SCREEN_ASPECT_RATIO = SCREEN_WIDTH /(float)SCREEN_HEIGHT;

        //Shader version used in ImGUI 
        const char* glsl_version ="#version 450";

        //Dummy Constructor / Destructor
        DisplayManager();
        ~DisplayManager();

        //Startup and shutdown sequence with detailed error messaging
        bool startUp();
        void shutDown();

        //Presenting the final rendered image to the display
        void bind();
        void swapDisplayBuffer();

    private:
        //Startup sub-functions
        bool startSDL();
        bool startOpenGL();
        bool createWindow();
        bool createGLContext();
        bool createImGuiContext();

        //All display related contexts
        SDL_Window   *mWindow;
        SDL_GLContext mContext;
};

#endif