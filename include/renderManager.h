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
        bool initSSBOs();
        bool loadShaders();

        //Internal Rendering functions
        void postProcess(const unsigned int start);
        void buildRenderQueue();

        //Todo:: move shaders to this to scene class?
        Shader *shaderAtlas[8]; //The number is kind of arbitrary for now

        //Pointers to data important for rendering
        SceneManager   * sceneLocator;
        Scene  *currentScene;
        Camera *sceneCamera;
        DisplayManager * screen;

        //Rendering miscs
        bool hasMoved = true;
        Quad canvas;

        //Stuff related to forward+
        ComputeShader *computeFrustrumPerTile;
        const unsigned int maxLights = 10000; // pretty overkill for sponza, but ok for testing
        unsigned int numLights;
        unsigned int frustrumSSBO, screenToViewSSBO, lightSSBO;
        unsigned int size, tileNumX, tileNumY, numTiles;

        //Render pipeline FBO's
        FrameBuffer multiSampledFBO;
        ResolveBuffer simpleFBO;
        QuadHDRBuffer pingPongFBO;
        DepthBuffer  dirShadowFBO;
        DepthBuffer   *pointLightShadowFBOs;

        //Old Framebuffers
        // GeometryBuffer gBuffer;
        // QuadHDRBuffer lightingBuffer, pingPong1, pingPong2;
        // DepthBuffer depthPrePass;
};
#endif