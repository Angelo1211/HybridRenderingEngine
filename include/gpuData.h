#ifndef GPUDATA_H
#define GPUTDATA_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-12
PURPOSE      : Data structures that will be used in the GPU as SSBO or other memory stuff.
*/

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct VolumeTileAABB{
    glm::vec4 minPoint;
    glm::vec4 maxPoint;
} frustrum;

struct ScreenToView{
    glm::mat4 inverseProjectionMat;
    unsigned int tileSizes[4];
    unsigned int screenWidth;
    unsigned int screenHeight;
    float sliceScalingFactor;
    float sliceBiasFactor;
}screen2View;

#endif