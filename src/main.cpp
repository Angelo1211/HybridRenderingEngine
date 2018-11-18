/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-05
PURPOSE      : Program Initialization and shutdown.
*/

#include "engine.h"
#include <stdio.h>

//Hybrid Rendering Engine
int main( int argc, char* args[] ){
    Engine HRE;
    if(HRE.startUp()){
        HRE.run();
    }
    else{
        printf("HRE could not initialize successfully. Shutting down.\n");
    }
    HRE.shutDown();

    return 0;
}