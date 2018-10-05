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
                    if( !createImGuiContext()){
                        success = false;
                    }
                    else{
                        //Imgui first frame setup
                        ImGui_ImplOpenGL3_NewFrame();
                        ImGui_ImplSDL2_NewFrame(mWindow);
                        ImGui::NewFrame();
                    }
                }
            }
        }
    }
    return success;
}

//Closes down the opengl context and sdl, also destroys window.
void DisplayManager::shutDown(){
    ImGui::EndFrame();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(mContext); 
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    SDL_Quit();
}

//Swaps the finished drawn buffer with the window bufffer.
//Also initializes a new frame for the gui renderer.
void DisplayManager::swapDisplayBuffer(){
    //Now that all data has been added to the frame we overlay the GUI onto it
    //Just before frame swap
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //Actual buffer swap
    SDL_GL_SwapWindow(mWindow);

    //Signaling beginning of frame to gui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mWindow);
    ImGui::NewFrame();
}

void DisplayManager::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

//Entry point to SDL
bool DisplayManager::startSDL(){
    if( SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError() );
        return  false;
    }
    return true;
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

        // No point in having a deplth buffer if you're using the default 
        // buffer for post processing
        // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        //Also set the default buffer to be sRGB 
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
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
            printf("GLAD could not load SDL Context.\n");
            return false;
        }
        else{
            //Vendor Information
            printf("Vendor:   %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Version:  %s\n", glGetString(GL_VERSION));

            //Init GL context settings
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

bool DisplayManager::createImGuiContext(){
    ImGuiContext * mGuiContext = ImGui::CreateContext();
    if( mGuiContext == nullptr){
        printf("Could not load IMGUI context!\n");
        return false;
    }
    else{
        ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();
        return true;
    }
}
