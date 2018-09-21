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
    // glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Build a render Queue for drawing multiple models
    //Also assigns the current camera to the software renderer
    //And gets info on the number of lights in the scene
    buildRenderQueue();

    //Temp matrix init TODO TODO TODO
    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 MV  = glm::mat4(1.0);

    //Activating shader and setting up uniforms that are constant
    currentShader->use();
    
    //Directional light
    currentShader->setVec3("dirLight.direction", glm::vec3(1.0f, -1.0f, 0.0f));
    currentShader->setVec3("dirLight.ambient",   glm::vec3(0.01f));
    currentShader->setVec3("dirLight.diffuse",   glm::vec3(0.1f));
    currentShader->setVec3("dirLight.specular",  glm::vec3(0.1f));

    //All the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(400.0f, 100.0f, 0.0f),
        glm::vec3(-400.0f, 100.0f, 0.0f),
        glm::vec3(400.0f, 400.0f, 200.0f),
        glm::vec3(-400.0f, 300.0f, -200.0f),
    };
    for(unsigned int i = 0; i < 4; ++i){
        std::string number = std::to_string(i);

        currentShader->setVec3(("pointLights[" + number + "].position").c_str(), pointLightPositions[i]);
        currentShader->setVec3(("pointLights[" + number + "].ambient").c_str(), glm::vec3(0.1f));
        currentShader->setVec3(("pointLights[" + number + "].diffuse").c_str(), glm::vec3(1.0f, 0.6f, 0.6f));
        currentShader->setVec3(("pointLights[" + number + "].specular").c_str(), glm::vec3(0.6f));
        currentShader->setFloat(("pointLights[" + number + "].constant").c_str(), 0.5f);
        currentShader->setFloat(("pointLights[" + number + "].linear").c_str(), 0.045f);
        currentShader->setFloat(("pointLights[" + number + "].quadratic").c_str(), 0.0007f);
    }

    while( !renderObjectQueue->empty() ){
        Model * currentModel = renderObjectQueue->front();

        //Light movement
        // float ang = 2.0f* M_PI * static_cast<float>(SDL_GetTicks()) / (16000.0f);
        // float radius = 1000.0f;
        // float X = std::sin(ang) * radius;
        // float Z = std::cos(ang) * radius;
        // glm::vec3 lightPos = glm::vec3(X, 100.0f , 0.0f);

        //Matrix setup
        MV  =   sceneCamera->viewMatrix * currentModel->getModelMatrix();
        MVP = sceneCamera->projectionMatrix * MV;

        //Shader setup stuff that changes every frame
        currentShader->setMat4("MVP", MVP);
        currentShader->setMat4("M", currentModel->getModelMatrix() );
        currentShader->setVec3("cameraPos_wS", sceneCamera->position);
        
        // currentShader->setMat4("MV", MV);
        // currentShader->setFloat("light.constant", 1.0f);
        // currentShader->setFloat("light.linear", 0.0014f);
        // currentShader->setFloat("light.quadratic", 0.000007f);
        // currentShader->setVec3("viewPos", sceneCamera->position);

        //Draw object
        currentModel->draw(*currentShader);
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










