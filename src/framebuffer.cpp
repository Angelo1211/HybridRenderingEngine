/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-20
*/
#include "frameBuffer.h"
#include "displayManager.h"
#include <stdio.h>

void FrameBuffer::bind(){
    glViewport(0,0,width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

//TODO:: This currently clears whatever framebuffer is bound, not the framebuffer that calls 
//this function
void FrameBuffer::clear(GLbitfield clearTarget, glm::vec3 clearColor){
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(clearTarget);
}

//Currently allows only for blit to one texture, not mrt blitting
void FrameBuffer::blitTo(const FrameBuffer &FBO, GLbitfield mask){
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO.frameBufferID);
    if(( mask & GL_COLOR_BUFFER_BIT ) == GL_COLOR_BUFFER_BIT){
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST );
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
}

//TODO:: include cases with width and height
void FrameBuffer::defaultInit(){
    width = DisplayManager::SCREEN_WIDTH;
    height = DisplayManager::SCREEN_HEIGHT;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

//Check if frame buffer initialized correctly
bool FrameBuffer::checkForCompleteness(){
    if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ){
        printf(" Failed to initialize the offscreen frame buffer!\n");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

/*
Framebuffer Characteristics
1. 1 Color, 1 Depth buffer
2. Color Buffer: screen width/height, not multisampled, HDR 
3. Depth Buffer: screen width/height, not multisampled, HDR 
*/
bool FrameBuffer::setupFrameBuffer(){
    defaultInit();

    texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL);
    depthBuffer    = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP);

    return checkForCompleteness();
}

/*
Framebuffer Characteristics
1. 1 Color, 1 Depth buffer
2. Color Buffer: screen width/height, multisampled, HDR 
3. Depth Buffer: screen width/height, multisampled, HDR 
*/
bool FrameBufferMultiSampled::setupFrameBuffer(){
    defaultInit();

    texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, MULT_2D_HDR_COL);
    depthBuffer    = Texture::genTextureDirectlyOnGPU(width, height, 0, MULT_2D_HDR_DEP);

    return checkForCompleteness();
}

/*
Framebuffer Characteristics
1. 2 Color, 1 Depth buffer
2. Color Buffer1: screen width/height, non multisampled, HDR 
3. Color Buffer2: screen width/height, non multisampled, HDR, clamped to edges
3. Depth Buffer: screen width/height, non multisampled, HDR 
*/
bool ResolveBuffer::setupFrameBuffer(){
    defaultInit();

    texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL);
    blurHighEnd    = Texture::genTextureDirectlyOnGPU(width, height, 1, SING_2D_HDR_COL_CLAMP);
    depthBuffer    = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP);

    return checkForCompleteness();
}

/*
Framebuffer Characteristics
1. 1 Color
2. Color Buffer: screen width/height, non multisampled, HDR , clamped to edges
*/
bool QuadHDRBuffer::setupFrameBuffer(){
    defaultInit();

    texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL_CLAMP);

    return checkForCompleteness();
}

/*
Framebuffer Characteristics
1. 1 depth
2. depth render Buffer: user set width/height, non multisampled, HDR 
*/
bool CaptureBuffer::setupFrameBuffer(unsigned int w, unsigned int h){
    width = w;
    height = h;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
     
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return checkForCompleteness();
}

void CaptureBuffer::resizeFrameBuffer(int resolution){
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
}

/*
Framebuffer Characteristics
1. 1 depth
2. depth Buffer: texture, user set width/height, non multisampled, HDR, border color set to black 
*/
bool DirShadowBuffer::setupFrameBuffer(unsigned int w, unsigned int h){
    width = w;
    height = h;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    depthBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP_BORDER);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    return checkForCompleteness();
}

/*
Framebuffer Characteristics
1. 1 depth cubemap
2. depth Buffer: texture, user set width/height, non multisampled, HDR, border color set to black 
*/
bool PointShadowBuffer::setupFrameBuffer(unsigned int w, unsigned int h){
    width = w;
    height = h;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    drawingTexture.generateCubeMap(width, height, SHADOW_MAP);
    depthBuffer = drawingTexture.textureID;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    return checkForCompleteness();
}


