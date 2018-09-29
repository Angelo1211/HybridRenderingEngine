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
#include <vector>

//High level render operations that shouldn't be done by the
//basic graphics lib.
class RenderManager{
    public:
        //Dummy constructors / Destructors
        RenderManager();
        ~RenderManager();

        //Gets scene and display info. Will be used to build render Queue
        bool startUp(DisplayManager &displayManager, SceneManager &sceneManager );
        void shutDown();

        //Performs all high level prep operations that the graphics library
        //Needs to do before beginning to draw each model in the scene.
        void render(const unsigned int start);

    private:
        //Setup
        bool loadShaders();
        bool setupQuad();

        //Internal Rendering
        void drawScene();
        void drawSceneFromLightPOV();
        void drawSkybox(const glm::mat4 &VP);
        void postProcess(const unsigned int start);
        void buildRenderQueue();

        Shader *shaderAtlas[4]; //The number is kind of arbitrary for now
        SceneManager   * sceneLocator;
        Camera *sceneCamera;
        DisplayManager * screen;
        Skybox *skybox;

        std::vector<Model*> *renderObjectQueue;        
        glm::mat4 lightSpaceMatrix;
        //OPENGL STUFF TODO TODO TODO

        FrameBuffer multiSampledFBO;
        ResolveBuffer simpleFBO;
        DepthBuffer  shadowFBO;
        unsigned int quadVAO;
        unsigned int quadVBO;

        //All the point lights
        const glm::vec3 pointLightPositions[4] = {
            glm::vec3(1100.0f, 150.0f, -400.0f),
            glm::vec3(1100.0f, 150.0f, 400.0f),
            glm::vec3(-1100.0f, 150.0f, 400.0f),
            glm::vec3(-1100.0f, 150.0f, -400.0f)
        };

        const glm::vec3 dirLightPosition = glm::vec3(1.0f, 4.0f, 1.0f);
};




#endif