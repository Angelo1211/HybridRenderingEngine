#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H
// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO 
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

        //Dummy Constructor / Destructor
        DisplayManager();
        ~DisplayManager();

        //Initializes SDL context and creates window according to above values
        bool startUp();
        void shutDown();

        //Swaps the pixel buffer with the window surface buffer and draws to screen
        void swapBuffers();

    private:
        //Wrappers for SDL init functions
        bool startSDL();
        // bool createWindow();
        // bool createScreenSurface();

        //Pointers to the SDL window and surface
        // SDL_Surface  *mSurface;
        SDL_Window   *mWindow;
        SDL_GLContext mContext;
};

#endif