#ifndef GEOMETRY_H
#define GEOMETRY_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-10
// ===============================

//Headers
#include "glm/glm.hpp"

struct Vertex {
   glm::vec3 position; 
   glm::vec3 normal; 
   glm::vec2 texCoords; 
};

#endif