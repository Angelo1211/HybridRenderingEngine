#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-13
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Includes
#include "displayManager.h"
#include "sceneManager.h"
#include "frameBuffer.h"
#include "skybox.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"
#include <vector>

class RenderManager{
    public:
        //Dummy constructors / Destructors
        RenderManager();
        ~RenderManager();

        //Gets scene and display info. Will be used to build render Queue
        bool startUp(DisplayManager &displayManager, SceneManager &sceneManager );
        void shutDown();

        void render(const unsigned int start);
    private:
        //Setup
        bool initFBOs();
        bool loadShaders();

        //Internal Rendering functions
        void postProcess(const unsigned int start);
        void buildRenderQueue();

        //Pointers to data important for rendering
        //Todo:: move this to scene class?
        Shader *shaderAtlas[7]; //The number is kind of arbitrary for now
        SceneManager   * sceneLocator;
        Scene  *currentScene;
        Camera *sceneCamera;
        DisplayManager * screen;

        unsigned int numLights;
        bool hasMoved = true;
        Quad canvas;

        //Render pipeline FBO's
        GeometryBuffer gBuffer;
        ResolveBuffer simpleFBO;
        // FrameBuffer multiSampledFBO;
        // DepthBuffer  dirShadowFBO;
        // PingPongBuffer pingPongFBO;
        // DepthBuffer   *pointLightShadowFBOs;

};
#endif