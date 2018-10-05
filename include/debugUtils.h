#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-10-04
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

#include "glad/glad.h"

#define glCheckError() glCheckError_(__FILE__, __LINE__)

namespace HREUtils{
    GLenum glCheckError_(const char *file, int line);
}

#endif