// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-02
// PURPOSE      : Program initialization and shutdown
// ===============================

#include "engine.h"
#include <stdio.h>

int main( int argc, char* args[] ){

    Engine HRE; //Hybrid Rendering Engine
    if(HRE.startUp()){
        HRE.run();
    }
    else{
        printf("HRE could not initialize successfully. Shutting down.\n");
    }
    HRE.shutDown();

    return 0;
}