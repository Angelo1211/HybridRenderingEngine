// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-13
// ===============================

//Includes
#include "renderManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//Dummy constructors / Destructors
RenderManager::RenderManager(){}
RenderManager::~RenderManager(){}

//Sets the internal pointers to the screen and the current scene and inits the software
//renderer instance. 
bool RenderManager::startUp(DisplayManager &displayManager,SceneManager &sceneManager ){
    currentShader = new Shader("basicShader.vert", "basicShader.frag");
    screen = &displayManager;
    sceneLocator = &sceneManager;
    // if( !initSoftwareRenderer() ){
    //     printf("Failed to initialize software Renderer!\n");
    //     return false;
    // }
    return true;
}

void RenderManager::shutDown(){
    delete currentShader;
    sceneCamera = nullptr;
    sceneLocator = nullptr;
    screen = nullptr;
    // renderInstance.shutDown();
}

void RenderManager::render(){
    //Clearing screen for rendering
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Build a render Queue for drawing multiple models
    //Also assigns the current camera to the software renderer
    //And gets info on the number of lights in the scene
    buildRenderQueue();

    //Temp matrix init TODO TODO TODO
    glm::mat4 MVP = glm::mat4(1.0);

    while( !renderObjectQueue->empty() ){
        Model * currentModel = renderObjectQueue->front();

        //Matrix setup
        MVP = sceneCamera->projectionMatrix * sceneCamera->viewMatrix * currentModel->getModelMatrix();

        //Shader setup
        currentShader->use();
        currentShader->setMat4("MVP", MVP);

        //Draw object
        renderObjectQueue->front()->draw(*currentShader);
        renderObjectQueue->pop();
    }

    //Drawing to the screen by swapping the window's surface with the
    //final buffer containing all rendering information
    screen->swapDisplayBuffer();

    //Set camera pointer to null just in case a scene change occurs
    sceneCamera = nullptr;
}

//Gets the list of visible models from the current scene
//Done every frame in case scene changes
void RenderManager::buildRenderQueue(){

    //set scene info
    Scene* currentScene = sceneLocator->getCurrentScene();
    
    //Set renderer camera
    sceneCamera = currentScene->getCurrentCamera();

    //Update the pointer to the list of lights in the scene
    // renderInstance.setSceneLights(currentScene->getCurrentLights(), currentScene->getLightCount() );

    //Get pointers to the visible model queue
    renderObjectQueue = currentScene->getVisiblemodels();
}

bool RenderManager::initSoftwareRenderer(){
    int w = DisplayManager::SCREEN_WIDTH;
    int h = DisplayManager::SCREEN_HEIGHT;
    return true;
    // return renderInstance.startUp(w,h);
}










