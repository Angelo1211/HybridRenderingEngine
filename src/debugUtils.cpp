/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-10-04
*/

#include "debugUtils.h"
#include <cstdio>
#include <string>

//Determine what OpenGL error you are facing specifically
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

//Finding the max sizes for compute units
void HREUtils::printComputeSizes(){
    int work_grp_cnt[3];
    int work_grp_size[3];

    //Counts
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    //Sizes
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max global (total) work group size x:%i y:%i z:%i\n",
        work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
        work_grp_size[0], work_grp_size[1], work_grp_size[2]);
}