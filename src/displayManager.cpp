/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-05 
*/

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

// Initialization sequence of all rendering related libraries such as SDL, GLAD and ImGUI.
bool DisplayManager::startUp(){
    if( !startSDL() ){
        return false;
    }

    if( !startOpenGL() ){
        return false;
    }

    if (!createWindow()){
        return false;
    }

    if( !createGLContext()){
        return false;
    }

    if( !createImGuiContext()){
        return false;
    }

    return true;
}

//Closes down all contexts and subsystems in the reverse initialization order
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

//Binding the display framebuffer for drawing and clearing it before rendering
void DisplayManager::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//Entry point to SDL
bool DisplayManager::startSDL(){
    if( SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError() );
        return  false;
    }
    return true;
}

//Loads the openGL library and sets the attributes of the future OpenGL context 
bool DisplayManager::startOpenGL(){
    if(SDL_GL_LoadLibrary(NULL) != 0 ){
        printf("Failed to initialize OpenGL. Error: %s\n", SDL_GetError() );
        return false;
    }

    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // No point in having a deplth buffer if you're using the default 
    // buffer only for post processing
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Also set the default buffer to be sRGB 
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    return true;
}

//Initializes SDL2 window object with the given width and heigh and marks it as
// an openGL enabled window;
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

/* 
1. Creates the OpenGL context using SDL
2. Inits GLAD functionality
3. Prints some vendor information
4. Sets some more GL context stuff  
5. Sets the glViewport to be the same size as the SDL window
 */
bool DisplayManager::createGLContext(){
    mContext = SDL_GL_CreateContext(mWindow);
    if(mContext == nullptr){
        printf("Could not create OpenGL context. Error: %s\n", SDL_GetError() );
        return false;
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        printf("GLAD could not load SDL Context.\n");
        return false;
    }

    //Printing some vendor Information
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    //Init GL context settings
    SDL_GL_SetSwapInterval(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //Setting the glViewport to be the size of the SDL window
    int w, h;
    SDL_GetWindowSize(mWindow, &w, &h);
    glViewport(0, 0, w, h);
    return true;
}

// Inits our GUI library and calls all init functions related to configuring it for use
// in OpenGL3+ and SDL2
bool DisplayManager::createImGuiContext(){
    ImGuiContext * mGuiContext = ImGui::CreateContext();
    if( mGuiContext == nullptr){
        printf("Could not load IMGUI context!\n");
        return false;
    }

    //Init and configure for OpenGL and SDL
    ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //Imgui first frame setup
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mWindow);
    ImGui::NewFrame();
    return true;
}
