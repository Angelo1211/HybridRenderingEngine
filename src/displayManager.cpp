// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// ===============================

//Includes
#include "displayManager.h"
#include <cstdio>
#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

//Dummy constructors/destructors
DisplayManager::DisplayManager(){}
DisplayManager::~DisplayManager(){}

//Initializes the window and obtains the surface on which we draw.
//Also first place where SDL and the OpenGL Context is initialized.
bool DisplayManager::startUp(){
    bool success = true;
    if( !startSDL() ){
        success = false;
    }
    else{
        if( !startOpenGL() ){
            success = false;
        }
        else{
            if (!createWindow()){
                success = false;
            }
            else{
                if( !createGLContext()){
                    success = false;
                }
                else{
                    ImGui::CreateContext();
                    ImGuiIO &io = ImGui::GetIO();
                    ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
                    ImGui_ImplOpenGL3_Init(glsl_version);
                    ImGui::StyleColorsDark();
                }
            }
        }
    }
    return success;
}

//Closes down the opengl context and sdl, also destroys window.
void DisplayManager::shutDown(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(mContext); 
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    SDL_Quit();
}

//Swaps the finished drawn buffer with the window bufffer.
void DisplayManager::swapDisplayBuffer(){
    SDL_GL_SwapWindow(mWindow);
}

//Entry point to OpenGL
bool DisplayManager::startOpenGL(){
    if(SDL_GL_LoadLibrary(NULL) != 0 ){
        printf("Failed to initialize OpenGL. Error: %s\n", SDL_GetError() );
        return false;
    }
    else{
        // Request an OpenGL 4.5 context (should be core)
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

        // Also request a depth buffer
        // No point in having a deplth buffer if your pipeline includes
        // post processing
        // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        //Also set the default buffer to be sRGB 
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    }
    return true;
}

//Entry point to SDL
bool DisplayManager::startSDL(){
    if( SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError() );
        return  false;
    }
    return true;
}

//TODO
bool DisplayManager::createWindow(){
    mWindow = SDL_CreateWindow( "Hybrid Renderering Engine", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if( mWindow == nullptr){
        printf("Could not create window. Error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
}

//Todo
bool DisplayManager::createGLContext(){
    mContext = SDL_GL_CreateContext(mWindow);
    if(mContext == nullptr){
        printf("Could not create OpenGL context. Error: %s\n", SDL_GetError() );
        return false;
    }
    else{
        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)){
            printf("GLAD could not load SDL Context.");
            return false;
        }
        else{
            //Vendor Information
            printf("Vendor:   %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Version:  %s\n", glGetString(GL_VERSION));

            //TODO: move this somewhere else
            SDL_GL_SetSwapInterval(0);
            // glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_FRAMEBUFFER_SRGB);
            int w, h;
            SDL_GetWindowSize(mWindow, &w, &h);
            glViewport(0, 0, w, h);
            return true;
        }
    }
}

SDL_Window* DisplayManager::getWindow(){
    return mWindow;
}