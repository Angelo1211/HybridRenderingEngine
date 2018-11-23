#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-13
PURPOSE      : 
SPECIAL NOTES: 
*/

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

        //Todo:: shaders should belong to a material not the rendermanager
        Shader depthPrePassShader, PBRClusteredShader, skyboxShader,
               highPassFilterShader, gaussianBlurShader, screenSpaceShader,
               dirShadowShader, pointShadowShader, fillCubeMapShader,
               convolveCubeMap, preFilterSpecShader, integrateBRDFShader;

        //Compute shaders also could be moved? 
        ComputeShader buildAABBGridCompShader, cullLightsCompShader;

        //Pointers to data important for rendering
        Camera *sceneCamera;
        Scene  *currentScene;
        DisplayManager *screen;
        SceneManager   *sceneLocator;

        //Rendering miscs
        Quad canvas;
        bool hasMoved = true;

        //Light initialization values
        unsigned int numLights;
        const unsigned int maxLights = 10000; // pretty overkill for sponza, but ok for testing
        const unsigned int maxLightsPerTile = 100;

        //Clustered Forward Setup
        const unsigned int gridSizeX = 16;
        const unsigned int gridSizeY =  9;
        const unsigned int gridSizeZ = 24;

        const unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ; 
        unsigned int sizeX, sizeY, cullDispatchX, cullDispatchY;

        //Buffer Setup
        unsigned int volumeGridAABB_SSBO, screenToViewSSBO;
        unsigned int lightSSBO, lightIndexList, lightGrid, lightIndexGlobalCount;

        //Render pipeline FBO's
        FrameBufferMultiSampled multiSampledFBO;
        ResolveBuffer simpleFBO;
        QuadHDRBuffer pingPongFBO;
        CaptureBuffer captureFBOBig, captureFBOSmall;
        DirShadowBuffer  dirShadowFBO;
        PointShadowBuffer   *pointLightShadowFBOs;
};
#endif