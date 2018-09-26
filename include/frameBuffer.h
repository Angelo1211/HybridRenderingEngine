#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H


struct FrameBuffer{
    void bind();
    bool setupFrameBuffer(bool isMultiSampled);
    bool multiSampled;
    unsigned int frameBufferID;
    unsigned int texColorBuffer;
    unsigned int renderBufferObject;
};

#endif