#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H
// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Includes
#include "SDL.h"

class DisplayManager{
    public:
        //The screen size is fixed and set at compile time along with other important
        //Display related values.
        const static int SCREEN_WIDTH  = 1280; //640 1280
        const static int SCREEN_HEIGHT = 720; //480 720
        const static int SCREEN_PITCH  = SCREEN_HEIGHT*sizeof(Uint32);
        constexpr static float SCREEN_ASPECT_RATIO = SCREEN_WIDTH /(float)SCREEN_HEIGHT;
        const char* glsl_version ="#version 450";

        //Dummy Constructor / Destructor
        DisplayManager();
        ~DisplayManager();

        // TODO
        //Initializes SDL context and creates window according to above values
        bool startUp();
        void shutDown();

        //TODO
        void swapDisplayBuffer();
        void bind();

    private:
        //Init functions
        bool startSDL();
        bool startOpenGL();
        bool createWindow();
        bool createGLContext();
        bool createImGuiContext();

        //Pointers to the SDL window and OpenGL Context
        SDL_Window   *mWindow;
        SDL_GLContext mContext;
};

#endif