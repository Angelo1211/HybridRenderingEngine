#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H
// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-10
// PURPOSE      : Manages the SDL screen itself and drawing to the display. 
// ===============================
// SPECIAL NOTES: Uses the old SDL1.2 backend because it's significantly faster
// for  pure software rendering. Using the new one would incur a 6ms penalty even on 
// empty frames. Probably has to do with sending data to the gpu every frame.
// ===============================

//Includes
#include "SDL.h"
#include "buffer.h"

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
        void swapBuffers(Buffer<Uint32> *pixelBuffer);

    private:
        //Wrappers for SDL init functions
        bool startSDL();
        bool createWindow();
        bool createScreenSurface();

        //Pointers to the SDL window and surface
        SDL_Surface  *mSurface;
        SDL_Window   *mWindow;
};

#endif