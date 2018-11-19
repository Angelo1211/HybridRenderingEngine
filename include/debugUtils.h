#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-10-04
PURPOSE      : A collection of useful debug functions. 
*/

#include "glad/glad.h"

#define glCheckError() glCheckError_(__FILE__, __LINE__)

namespace HREUtils{
    GLenum glCheckError_(const char *file, int line);
    void   printComputeSizes();
}

#endif