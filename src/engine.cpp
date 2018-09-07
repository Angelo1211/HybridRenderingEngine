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
    
    //Raw shader data
    // const char *vertexSource = "#version 450 core\n"
    // "layout (location = 0) in vec3 aPos;\n"
    // "out vec4 vertexColor;\n"
    // "void main()\n"
    // "{\n"
    // "   gl_Position = vec4(aPos, 1.0);\n"
    // "   vertexColor = vec4(0.5, 0.0, 0.5, 1.0);\n"
    // "}\0";

    const char *vertexSource = "#version 450 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";

    const char *fragmentSource = "#version 450 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 uniformColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor  = uniformColor;\n"
    "}\n\0";

     //Vertex shader stuff
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed %s\n", infoLog );
    }

    //Fragment shader stuff
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed %s\n", infoLog );
    }

    //Linking shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader Linking failed %s\n", infoLog );
    }

    //Deleting shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SDL_Event event;
    
    //Vertex data 
    float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
    };  


    //Init VBO, VAO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    //Bind vertex array object first
    glBindVertexArray(VAO);

    //Bind and set vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Bind and set element buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Set the pointers to the beginning of the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Unbinding VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Unbinding vertex array
    glBindVertexArray(0);
    


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

        float greenVal = (sin(start/1000.0f)+1.0f) * 2.0f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "uniformColor");
        glUseProgram(shaderProgram);
        glUniform4f(vertexColorLocation, 0.0f, greenVal, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
       
        gDisplayManager.update();






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