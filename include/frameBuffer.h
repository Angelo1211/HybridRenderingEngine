#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-20
PURPOSE      : 
SPECIAL NOTES: 
*/

/* TODO TODO TODO 
You should refactor away some of the texture creation code into the texture class to
avoid cluttering so much of the frame buffer creation with texture stuff.

Also having different structs for each kind of framebuffer is somewhat wwasteful
since you also have initialization for each one. I'm not really sure how to 
compress this though since each initialization is genuinely different for each 
kind of buffer. TODO?
*/

//Includes
#include "texture.h"
#include "cubeMap.h"
#include "glad/glad.h"

enum FrameBufferType{
    MULTISAMPLED_FORWARD_FB,
    RESOLVE_FB,
    HDR_FB,
    DEPTH_FB,
    GEOMETRY_FB,
    CAPTURE_FB
};

struct ResolveBuffer;

struct FrameBuffer{
    void bind();
    void clear(GLbitfield clearTarget, glm::vec3 clearColor);
    void blitTo(const ResolveBuffer &FBO, GLbitfield mask);
    bool setupFrameBuffer(bool isMultiSampled);

    bool multiSampled;
    unsigned int frameBufferID, texColorBuffer, depthBuffer;
    int width, height;
};

struct ResolveBuffer : public FrameBuffer{
    bool setupFrameBuffer();
    unsigned int blurHighEnd;
};

struct QuadHDRBuffer : public FrameBuffer{
    bool setupFrameBuffer();
};

//Needs to be refactored at some point
struct DepthBuffer{
    CubeMap drawingTexture;
    void bind();
    bool setupFrameBuffer(unsigned int width, unsigned int height, bool omnidirectional);
    unsigned int depthMap, frameBufferID ;
    int width, height;
};

//Needs to be refactored at some point
struct GeometryBuffer{
    void bind();
    bool setupFrameBuffer();
    unsigned int frameBufferID, positionBuffer, normalsBuffer, albedoSpecBuffer, zBufferWStencil;
    int width, height;
};

struct CaptureBuffer {
    void bind();
    bool setupFrameBuffer(unsigned int w, unsigned int h);
    unsigned int frameBufferID, captureRBO;
    int width, height;
};
#endif