// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// ===============================

//Headers
#include "engine.h"
#include <stdio.h>
#include "SDL.h"
#include "glad/glad.h"
#include <math.h>

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

    }
    // else{
    //     //Initis scene manager and loads default scene
    //     if( !gSceneManager.startUp() ){
    //         success = false;
    //         printf("Failed to initialize scene manager.\n");
    //     }
    //     else{
    //         //Initializes rendererer manager, which is in charge of high level
    //         //rendering tasks (render queue, locating render scene etc)
    //         //It gets passed references to the other major subsystems for use later
    //         //on setup of the render queue.
    //         if( !gRenderManager.startUp(gDisplayManager, gSceneManager) ){
    //             success = false;
    //             printf("Failed to initialize render manager.\n");
    //         }
    //         else{
    //             //Initializing input manager that manages all mouse, keyboard and
    //             //mousewheel input. It needs access to the scene manager to apply the
    //             //changes on the scene caused by user input. 
    //             if ( !gInputManager.startUp(gSceneManager) ){
    //                 success = false;
    //                 printf("Failed to initialize input manager.\n");
    //             }
    //         }
    //     }
    // }
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
    
    //GPU Memory stuff
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  
    //Init VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    //Init VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);


    //Copy data into VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Set the pointers to the beginning of the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    



    //Vertex shader stuff
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_ARRAY);

    const char *vertexSource = "#version 450 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);


    //Fragment shader stuff
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *fragmentSource = "#version 450 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    //Shader program stuff
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


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
        double intpart;
        glClearColor(0.0f, 0.5f , 1.0f, 0.0f);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gDisplayManager.update();






        //Monitoring time taken per frame to gauge engine performance
        deltaT = SDL_GetTicks() - start;
        printf("%2.1d: Frame elapsed time (ms):%d\n",count, deltaT);
        total += deltaT;
    }
    printf("Closing down engine.\n");
    printf("Average frame time over %2.1d frames:%2.fms.\n", count, total/(float)count);
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