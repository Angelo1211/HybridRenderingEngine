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
#include "skybox.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include <queue>

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
        bool buildFrameBuffer();
        bool loadShaders();
        bool setupQuad();

        //Internal Rendering
        void drawScene();
        void drawSkybox(const glm::mat4 &VP);
        void postProcess(const unsigned int start);
        void buildRenderQueue();

        Shader *shaderAtlas[3]; //The number is kind of arbitrary for now
        SceneManager   * sceneLocator;
        Camera *sceneCamera;
        DisplayManager * screen;
        Skybox *skybox;

        std::queue<Model*> *renderObjectQueue;        

        //OPENGL STUFF TODO TODO TODO
        unsigned int quadVAO;
        unsigned int quadVBO;
        unsigned int frameBuffer;
        unsigned int texColorBuffer;
        unsigned int renderBufferObject;
};




#endif