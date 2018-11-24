/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-05 
*/

//Headers
#include "engine.h"
#include "SDL.h"
#include <stdio.h>

//Dummy constructors and destructors
Engine::Engine(){}
Engine::~Engine(){}

bool Engine::startUp(){
    unsigned int start = SDL_GetTicks(); //Could probably be its own timer class 

    //Start up of all SDL and opengl Display related content
    if(!gDisplayManager.startUp()){
        printf("Failed to initialize window display manager.\n");
        return false;
    }

    // Inits scene manager and loads default scene
    if(!gSceneManager.startUp()){
        printf("Failed to initialize scene manager.\n");
        return false;
    }

    //Initializes rendererer manager, which is in charge of high level
    //rendering tasks (render queue, locating render scene etc)
    //It gets passed references to the other major subsystems for use later
    if (!gRenderManager.startUp(gDisplayManager, gSceneManager)){
        printf("Failed to initialize Render manager.\n");
        return false;
    }

    //Initializing input manager that manages all mouse, keyboard and
    //mousewheel input. It needs access to the scene manager to apply the
    //changes on the scene caused by user input.
    if (!gInputManager.startUp(gSceneManager)){
        printf("Failed to initialize input manager.\n");
        return false;
    }

    //Want to keep track of how much time the whole loading process took
    unsigned int deltaT = SDL_GetTicks() - start;
    printf("(Load time: %ums)\n",deltaT);
    return true;
}

//Closing in opposite order to avoid dangling pointers
void Engine::shutDown(){
    gInputManager.shutDown();
    printf("Closed input manager.\n");

    gRenderManager.shutDown();
    printf("Closed renderer manager.\n");
    
    gSceneManager.shutDown();
    printf("Closed scene manager.\n");
    
    gDisplayManager.shutDown();
    printf("Closed display manager.\n");
}

//Runs main application loop 
void Engine::run(){

    //Early exit or other shared data flags
    bool done = false;

    //Iteration and time keeping counters
    int count = 0;
    unsigned int deltaT = SDL_GetTicks();
    unsigned int start = 0;;
    unsigned int total = 0;
    printf("\nEntered Main Loop!\n");

    while(!done){
        ++count;
        start = SDL_GetTicks(); 

        //Handle all user input
        //Any changes to the scene are directly sent to the respective objects in
        //the scene class. Also sets exit flag based on user input.
        gInputManager.processInput(done, deltaT);

        //Update all models, camera and lighting in the current scene
        gSceneManager.update(deltaT);

        gRenderManager.render(start);

        //Obtaining deltaT for any 
        deltaT = SDL_GetTicks() - start;
        total += deltaT;
    }
    
    //Average performance over the whole loop (excludes load time costs)
    printf("\nPerformance Stats:\n------------------\n");
    printf("Average frame time over %2.1d frames:%2.fms.\n\n", count, total/(float)count);
    printf("Closing down engine...\n");
}


        

