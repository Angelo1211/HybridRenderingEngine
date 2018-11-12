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
        Shader *shaderAtlas[9]; //The number is kind of arbitrary for now


        //Pointers to data important for rendering
        SceneManager   * sceneLocator;
        Scene  *currentScene;
        Camera *sceneCamera;
        DisplayManager * screen;

        //Rendering miscs
        bool hasMoved = true;
        Quad canvas;


        //Light initialization values
        const unsigned int maxLights = 10000; // pretty overkill for sponza, but ok for testing
        const unsigned int maxLightsPerTile = 100;
        unsigned int numLights;

        //Clustered Forward Setup
        ComputeShader *computeGridAABB, *cullLightsAABB, *computeDepths;
        const unsigned int gridSizeX = 16;
        const unsigned int gridSizeY =  9;
        const unsigned int gridSizeZ = 24;

        const unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ; 
        unsigned int sizeX, sizeY, cullDispatchX, cullDispatchY;

        //Buffer Setup
        unsigned int volumeGridAABB_SSBO, screenToViewSSBO;
        unsigned int lightSSBO, lightIndexList, lightGrid, lightIndexGlobalCount;

        //Render pipeline FBO's
        FrameBuffer multiSampledFBO;
        CaptureBuffer captureFBO;
        ResolveBuffer simpleFBO;
        QuadHDRBuffer pingPongFBO;
        DepthBuffer  dirShadowFBO;
        DepthBuffer   *pointLightShadowFBOs;

        //Old Framebuffers
        // GeometryBuffer gBuffer;
        // QuadHDRBuffer lightingBuffer, pingPong1, pingPong2;
        // DepthBuffer depthPrePass;

        //Stuff related to forward+
        // ComputeShader *computeFrustrumPerTile, *cullLights, *computeDepths;
};
#endif