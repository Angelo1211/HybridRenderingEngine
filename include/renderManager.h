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

        //Internal Rendering
        // void drawScene();
        // void drawPointShadow(int i );
        // void drawDirShadow();
        void postProcess(const unsigned int start);
        void buildRenderQueue();

        Shader *shaderAtlas[7]; //The number is kind of arbitrary for now
        SceneManager   * sceneLocator;
        Scene  *currentScene;
        Camera *sceneCamera;
        DisplayManager * screen;

        // std::vector<Model*> *renderObjectQueue;        

        //OPENGL STUFF TODO TODO TODO
        bool hasMoved = true;
        Quad canvas;

        FrameBuffer multiSampledFBO;
        ResolveBuffer simpleFBO;
        DepthBuffer  dirShadowFBO;
        PingPongBuffer pingPongFBO;
        DepthBuffer   *pointLightShadowFBOs;

        // TODO: MOVE LIGHT INFO INTO ITS OWN CLASS (POSSIBLY ON SCENE LOAD)
        // const int shadowMapResolution = 2 * 1024;
        float exposure = 1.0f;
        unsigned int numLights;
        int amount = 0;

        // const glm::vec3 pointLightPositions[4] = {
        //     glm::vec3(1100.0f, 200.0f, -400.0f),
        //     glm::vec3(1100.0f, 200.0f, 400.0f),
        //     glm::vec3(-1150.0f, 200.0f, 400.0f),
        //     glm::vec3(-1150.0f, 200.0f, -400.0f)
        // };

        // const glm::vec3 pointLightColor[4] = {
        //     glm::vec3(1.0f, 0.0f, 0.0f),
        //     glm::vec3(0.0f, 1.0f, 0.0f),
        //     glm::vec3(0.0f, 0.0f, 1.0f),
        //     glm::vec3(0.0f, 1.0f, 1.0f)
        // };
};
#endif