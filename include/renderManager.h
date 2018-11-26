#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-13
PURPOSE      : Setting up opengl, pre-processing data for rendering, shadow mapping, post processing 
               and shader initialization. It is the main interface that is called for rendering.
SPECIAL NOTES: This is the core of the program/project and where most of the work is still to be done.
               Starting with the static shadow mapping that we currently have implemented, that's 
               gotta change asap. The truth is I still have to wrap my head around how virtual texture
               tiling works before I can begin the refactor. There is also a pretty big argument to be made
               to move some of the shaders somewhere else, but that would probably be done best when
               the new material model comes in. 
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

        //The core rendering loop of the engine. The steps that it follow are indicated on the cpp file 
        //but it's just a very vanilla Forward+ clustered renderer with barely any bells and whistled or 
        //optimizations. In the future, (the magical land where all projects are complete) I plan on
        //heavily optimizing this part of the program along the lines of the 2014 talk, "beyond porting"
        //But in the mean-time it uses pretty basic an naive openGL.
        void render(const unsigned int start);
    private:
        //Internal initialization functions
        bool initFBOs();
        bool initSSBOs();
        bool loadShaders();
        //TODO:: rewrite shadow mapping to be dynamic and fast and make use of cluster shading
        //and some new low driver overhead stuff I've been reading about
        bool preProcess(); 

        //Functions used to break up the main render function into more manageable parts.
        void postProcess(const unsigned int start);

        //Todo:: shaders should belong to a material not the rendermanager
        Shader depthPrePassShader, PBRClusteredShader, skyboxShader,
               highPassFilterShader, gaussianBlurShader, screenSpaceShader,
               dirShadowShader, pointShadowShader, fillCubeMapShader,
               convolveCubeMap, preFilterSpecShader, integrateBRDFShader;

        //TODO::Compute shaders don't have a strong a case as regular shaders to be made a part of 
        //other classes, since they feel more like static functions of the renderer than methods that
        //are a part of certain objects. 
        ComputeShader buildAABBGridCompShader, cullLightsCompShader;

        //Pointers to the scene and its contents which contains all the geometry data
        //that we will be rendering rendering
        Camera *sceneCamera;
        Scene  *currentScene;
        DisplayManager *screen;
        SceneManager   *sceneLocator;

        //The canvas is an abstraction for screen space rendering. It helped me build a mental model
        //of drawing at the time but I think it is now unecessary sinceI feel much more comfortable with
        //compute shaders and the inner workings of the GPU. 
        Quad canvas;

        //The variables that determine the size of the cluster grid. They're hand picked for now, but
        //there is some space for optimization and tinkering as seen on the Olsson paper and the ID tech6
        //presentation.
        const unsigned int gridSizeX = 16;
        const unsigned int gridSizeY =  9;
        const unsigned int gridSizeZ = 24;
        const unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ; 
        unsigned int sizeX, sizeY;

        unsigned int numLights;
        const unsigned int maxLights = 1000; // pretty overkill for sponza, but ok for testing
        const unsigned int maxLightsPerTile = 50;

        //Shader buffer objects, currently completely managed by the rendermanager class for creation
        //using and uploading to the gpu, but they should be moved somwehre else to avoid bloat
        unsigned int AABBvolumeGridSSBO, screenToViewSSBO;
        unsigned int lightSSBO, lightIndexListSSBO, lightGridSSBO, lightIndexGlobalCountSSBO;

        //Render pipeline FrameBuffer objects. I absolutely hate that the pointlight shadows have distinct
        //FBO's instead of one big one. I think we will take the approach that is outlined on the Id tech 6 talk
        //and use a giant texture to store all textures. However, since this require a pretty substantial rewrite
        //of the illumination code I have delayed this until after the first official github release of the
        //project.
        ResolveBuffer simpleFBO;
        CaptureBuffer captureFBO;
        QuadHDRBuffer pingPongFBO;
        DirShadowBuffer  dirShadowFBO;
        FrameBufferMultiSampled multiSampledFBO;
        PointShadowBuffer   *pointLightShadowFBOs;
};
#endif