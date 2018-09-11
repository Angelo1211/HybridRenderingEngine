// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// ===============================

//Headers
#include "engine.h"
#include "SDL.h"
#include "glad/glad.h"
#include "shader.h"
#include "model.h"

#include <math.h>
#include <stdio.h>
//Dummy constructors and destructors
Engine::Engine(){}
Engine::~Engine(){}


bool Engine::startUp(){
    bool success = true;
    //Start up of all SDL Display related content
    if( !gDisplayManager.startUp() ){
        success = false;
        printf("Failed to initialize window display manager.\n");
    }
    else{
        //Initis scene manager and loads default scene
        // if( !gSceneManager.startUp() ){
        //     success = false;
        //     printf("Failed to initialize scene manager.\n");
        // }
        // else{
        //     //Initializes rendererer manager, which is in charge of high level
        //     //rendering tasks (render queue, locating render scene etc)
        //     //It gets passed references to the other major subsystems for use later
        //     //on setup of the render queue.
        //     if( !gRenderManager.startUp(gDisplayManager, gSceneManager) ){
        //         success = false;
        //         printf("Failed to initialize render manager.\n");
        //     }
        //     else{
        //         //Initializing input manager that manages all mouse, keyboard and
        //         //mousewheel input. It needs access to the scene manager to apply the
        //         //changes on the scene caused by user input. 
        //         if ( !gInputManager.startUp(gSceneManager) ){
        //             success = false;
        //             printf("Failed to initialize input manager.\n");
        //         }
        //     }
        // }
    }
    return success;
}

//Closing in opposite order to avoid dangling pointers
void Engine::shutDown(){
    // gInputManager.shutDown();
    // printf("Closed input manager.\n");

    // gRenderManager.shutDown();
    // printf("Closed renderer manager.\n");
    
    // gSceneManager.shutDown();
    // printf("Closed Scene manager.\n");
    
     gDisplayManager.shutDown();
    printf("Closed display manager.\n");
}

//Runs main application loop 
void Engine::run(){
    //Main flags
    bool done = false;

    //Iteration and time keeping counters
    int count = 0;
    unsigned int deltaT = 0;
    unsigned int start = 0;;
    unsigned int total = 0;

    //Temp stuff ignore for now
    //---------------------------------------------------------------------------------------
    
    SDL_Event event;

    //Init Shader
    Shader basicShader("basicShader.vert", "basicShader.frag");
    
    //Loading model
    Model testModel("../scenes/teapot_mesh.obj");
    
    //---------------------------------------------------------------------------------------
    printf("Entered Main Loop!\n");
    while(!done){
        ++count;
        start = SDL_GetTicks(); //Could probably be its own timer class, but we're keeping things simple here
        
        //TEMP input processing
        while( SDL_PollEvent(&event) ){
            if(event.type == SDL_QUIT){
                done = true;
            }
        }

        //TEMP Rendering
        glClearColor(0.0f, 0.5f , 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        basicShader.use();
        testModel.draw(basicShader);


        //Swap buffers 
        gDisplayManager.swapDisplayBuffer();

        //Monitoring time taken per frame to gauge engine performance
        deltaT = SDL_GetTicks() - start;
        printf("%2.1d: Frame elapsed time (ms):%d\n",count, deltaT);
        total += deltaT;
    }
    printf("\nPerformance Stats:\n------------------\n");
    printf("Average frame time over %2.1d frames:%2.fms.\n\n", count, total/(float)count);
    printf("Closing down engine...\n");
}


 // //Handle all user input
        // //Any changes to the scene are directly sent to the respective objects in
        // //the scene class. Also sets exit flag based on user input.
        // gInputManager.processInput(done, deltaT);
        
        // //Update all models, camera and lighting in the current scene
        // //Also performs view frustrum culling to determine which objects aare visible
        // gSceneManager.update(deltaT);

        // //Contains the render setup and actual software rendering loop
        // gRenderManager.render();
