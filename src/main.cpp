// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-02
// PURPOSE      : Program initialization and shutdown
// ===============================

#include "engine.h"
#include <stdio.h>

//Hybrid Rendering Engine
int main( int argc, char* args[] ){

    if(Engine::instance().startUp()){
        Engine::instance().run();
    }
    else{
        printf("HRE could not initialize successfully. Shutting down.\n");
    }
    Engine::instance().shutDown();

    return 0;
}