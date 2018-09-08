// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// ===============================

//Includes
#include "displayManager.h"
#include <cstdio>
#include "glad/glad.h"

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
        // if( !createWindow() ){
        //     success = false;
        // }
        SDL_GL_LoadLibrary(NULL);

        // Request an OpenGL 4.5 context (should be core)
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

        // Also request a depth buffer
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        mWindow = SDL_CreateWindow(
            "Hybrid Renderer", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL
        );
        
        mContext = SDL_GL_CreateContext(mWindow);

        //Checking OpenGL properties
        gladLoadGLLoader(SDL_GL_GetProcAddress);

        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n", glGetString(GL_VERSION));        

        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        int w,h;
        SDL_GetWindowSize(mWindow, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);


    }
    return success;
}

//Closes down sdl and destroys window.
//SDL surface is also destroyed in the call to destroy window
void DisplayManager::shutDown(){
    SDL_GL_DeleteContext(mContext); 
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    SDL_Quit();
}

//Applies the rendering results to the window screen by copying the pixelbuffer values
//to the screen surface.
void DisplayManager::update(){

    SDL_GL_SwapWindow(mWindow);
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
// bool DisplayManager::createWindow(){
//     mWindow = SDL_CreateWindow( "SoftwareRenderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
//     if( mWindow == nullptr){
//         printf("Could not create window. Error: %s\n", SDL_GetError() );
//         return false;
//     }
//     return true;
// }

//Gets the screen surface
//I know this is "Old" SDL and it's not really recommended anymore
//But given that I am doing 100% cpu based rendering it makes sense
// //After all I'm not usin any of the new functionality
// bool DisplayManager::createScreenSurface(){
//     mSurface = SDL_GetWindowSurface(mWindow);
//     if(mSurface == NULL){
//         printf("Could not create window surface. Error: %s\n", SDL_GetError());
//         return false;
//     }
//     return true;
// }