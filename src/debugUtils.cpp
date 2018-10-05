// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-10-04
// ===============================

#include "debugUtils.h"
#include <cstdio>
#include <string>

GLenum HREUtils::glCheckError_(const char *file, int line){
    GLenum errorCode;
    while((errorCode = glGetError()) != GL_NO_ERROR){
        std::string error;
        switch(errorCode){
            case GL_INVALID_ENUM: error = "Invalid Enum"; break;
            case GL_INVALID_VALUE: error = "Invalid value"; break;
            case GL_INVALID_OPERATION: error = "Invalid operation"; break;
            case GL_STACK_OVERFLOW: error = "stack overflow"; break;
            case GL_STACK_UNDERFLOW: error = "stack underflow"; break;
            case GL_OUT_OF_MEMORY: error = "out of memory"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "invalid framebuffer operation"; break;
        }
        printf("%s, | %s ( %d ) \n", error.c_str(), file, line);
    }
    return errorCode;
}