#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

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


struct DepthBuffer{
    void bind();
    bool setupFrameBuffer(unsigned int width, unsigned int height);
    unsigned int depthMap, frameBufferID ;
    int width, height;
};


#endif