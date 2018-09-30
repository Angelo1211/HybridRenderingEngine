#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/* TODO TODO TODO 
You should refactor away some of the texture creation code into the texture class to
avoid cluttering so much of the frame buffer creation with texture stuff.
*/

#include "texture.h"

struct ResolveBuffer;

struct FrameBuffer{
    void bind();
    void blitTo(const ResolveBuffer &FBO);
    bool setupFrameBuffer(bool isMultiSampled);

    bool multiSampled;
    unsigned int frameBufferID, texColorBuffer, renderBufferObject;
    int width, height;
};

struct ResolveBuffer : public FrameBuffer{
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


#endif