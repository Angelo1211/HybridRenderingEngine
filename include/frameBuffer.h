#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-20
PURPOSE      : To manage where and how the GPU draws the models. Framebuffers serve as render targets
               for shadows, shading, post processing, blurring and pre-processing
SPECIAL NOTES: Some structs are so thin, it probably would make sense to just make them functions 
               instead of structs. But for now I'll keep them like this to maintain consistency.
*/

//Includes
#include "texture.h"
#include "cubeMap.h"
#include "glad/glad.h"

//Main rendering buffers
struct FrameBuffer{
    void bind();
    void clear(GLbitfield clearTarget, glm::vec3 clearColor);
    void blitTo(const FrameBuffer &FBO, GLbitfield mask);

    void defaultInit();
    bool setupFrameBuffer();
    bool checkForCompleteness();

    int width, height;
    unsigned int frameBufferID; 
    unsigned int texColorBuffer, depthBuffer;
};
struct FrameBufferMultiSampled : public FrameBuffer{
    bool setupFrameBuffer();
};

//Post Processing buffers
struct ResolveBuffer : public FrameBuffer{
    bool setupFrameBuffer();
    unsigned int blurHighEnd;
};
struct QuadHDRBuffer : public FrameBuffer{
    bool setupFrameBuffer();
};
struct CaptureBuffer : public FrameBuffer {
    void resizeFrameBuffer(int resolution);
    bool setupFrameBuffer(unsigned int w, unsigned int h);
};

//Shadow mapping frame buffers
struct DirShadowBuffer : public FrameBuffer{
    bool setupFrameBuffer(unsigned int w, unsigned int h);
};
struct PointShadowBuffer : public FrameBuffer{
    CubeMap drawingTexture;
    bool setupFrameBuffer(unsigned int w, unsigned int h);
};

#endif