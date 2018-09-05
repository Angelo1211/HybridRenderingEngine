// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-10
// ===============================

//Includes
#include "displayManager.h"
#include <cstdio>
#include <cstring>

//Dummy constructors/destructors
DisplayManager::DisplayManager(){}
DisplayManager::~DisplayManager(){}

//Initializes the window and obtains the surface on which we draw.
//Also first place where SDL is initialized. 
bool DisplayManager::startUp(){
    bool success = true;
    if( !startSDL() ){
        success = false;
    }
    else{
        if( !createWindow() ){
            success = false;
        }
        else{
            if( !createScreenSurface() ){
                success = false;
            }
        }
    }
    return success;
}

//Closes down sdl and destroys window.
//SDL surface is also destroyed in the call to destroy window
void DisplayManager::shutDown(){
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    SDL_Quit();
}

//Applies the rendering results to the window screen by copying the pixelbuffer values
//to the screen surface.
void DisplayManager::swapBuffers(Buffer<Uint32> *pixels){
    //Allows surface editing 
    SDL_LockSurface(mSurface);

    //Copy pixels buffer resuls to screen surface
    std::memcpy(mSurface->pixels, pixels->buffer, pixels->mHeight*pixels->mPitch);
    SDL_UnlockSurface(mSurface);

    //Apply surface changes to window
    SDL_UpdateWindowSurface(mWindow);

}

//Entry point to SDL
bool DisplayManager::startSDL(){
    if( SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError() );
        return  false;
    }
    return true;
}

//Inits window with the display values crated at compile time
bool DisplayManager::createWindow(){
    mWindow = SDL_CreateWindow( "SoftwareRenderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if( mWindow == nullptr){
        printf("Could not create window. Error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
}

//Gets the screen surface
//I know this is "Old" SDL and it's not really recommended anymore
//But given that I am doing 100% cpu based rendering it makes sense
//After all I'm not usin any of the new functionality
bool DisplayManager::createScreenSurface(){
    mSurface = SDL_GetWindowSurface(mWindow);
    if(mSurface == NULL){
        printf("Could not create window surface. Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}