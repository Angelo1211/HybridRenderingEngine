#ifndef GPUDATA_H
#define GPUDATA_H

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
    unsigned int tileSizeX;
    unsigned int tileSizeY;
    unsigned int tileSizeZ;
    unsigned int padding1;
    glm::vec2 tilePixelSize;
    glm::vec2 viewPixelSize;
    float sliceScalingFactor;
    float sliceBiasFactor;
    unsigned int padding2;
    unsigned int padding3;
}screen2View;

#endif