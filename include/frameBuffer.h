#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H


struct FrameBuffer{
    void bind();
    void blitTo(const FrameBuffer &FBO);
    bool setupFrameBuffer(bool isMultiSampled);

    bool multiSampled;
    unsigned int frameBufferID, texColorBuffer, renderBufferObject;
    int width, height;
};

#endif