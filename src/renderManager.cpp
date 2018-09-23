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
bool RenderManager::startUp(DisplayManager &displayManager, SceneManager &sceneManager ){
    screen = &displayManager;
    sceneLocator = &sceneManager;

    //I know this is uneccessary but it might be useful if I add more startup functions
    //in the future
    if( !buildFrameBuffer() ){
        return false;
    }
    else{
        if (!loadShaders()){
            printf("All shaders failed to compile! \n");
            return false;
        }
        else{
            if( !setupQuad()){
                return false;
            }
        }
    }
    return true;
}

void RenderManager::shutDown(){
    delete shaderAtlas[0];
    delete shaderAtlas[1];
    delete [] shaderAtlas;
    sceneCamera  = nullptr;
    sceneLocator = nullptr;
    screen = nullptr;
}

bool RenderManager::setupQuad(){
    const float quadVertices[] = {
        //positions //texCoordinates
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    //OpenGL postprocessing quad setup
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    //VBO initialization
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    //Quad position pointer initialization in attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    //Quad texcoords pointer initialization in attribute array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);


    //TODO add error checking
    return true;
}

bool RenderManager::loadShaders(){
    shaderAtlas[0] = new Shader("basicShader.vert", "basicShader.frag");
    shaderAtlas[1] = new Shader("screenShader.vert", "screenShader.frag");

    return ( shaderAtlas[0] != nullptr ) && ( shaderAtlas[1] != nullptr );
}

//Here we do the offscreen rendering for hte whole scene
void RenderManager::drawScene(){
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glEnable(GL_DEPTH_TEST);
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
    shaderAtlas[0]->use();
    
    //Directional light
    shaderAtlas[0]->setVec3("dirLight.direction", glm::vec3(1.0f, -1.0f, 0.0f));
    shaderAtlas[0]->setVec3("dirLight.ambient",   glm::vec3(0.05f));
    shaderAtlas[0]->setVec3("dirLight.diffuse",   glm::vec3(0.4f));
    shaderAtlas[0]->setVec3("dirLight.specular",  glm::vec3(0.4f));

    //All the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(400.0f, 100.0f, 0.0f),
        glm::vec3(-400.0f, 100.0f, 0.0f),
        glm::vec3(400.0f, 400.0f, 200.0f),
        glm::vec3(-400.0f, 300.0f, -200.0f),
    };
    for(unsigned int i = 0; i < 4; ++i){
        std::string number = std::to_string(i);

        shaderAtlas[0]->setVec3(("pointLights[" + number + "].position").c_str(), pointLightPositions[i]);
        shaderAtlas[0]->setVec3(("pointLights[" + number + "].ambient").c_str(), glm::vec3(0.1f));
        shaderAtlas[0]->setVec3(("pointLights[" + number + "].diffuse").c_str(), glm::vec3(1.0f, 0.6f, 0.6f));
        shaderAtlas[0]->setVec3(("pointLights[" + number + "].specular").c_str(), glm::vec3(0.6f));
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].constant").c_str(), 1.0f);
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].linear").c_str(), 0.0014f);
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].quadratic").c_str(), 0.000007f);
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
        MV  = sceneCamera->viewMatrix * currentModel->getModelMatrix();
        MVP = sceneCamera->projectionMatrix * MV;

        //Shader setup stuff that changes every frame
        shaderAtlas[0]->setMat4("MVP", MVP);
        shaderAtlas[0]->setMat4("M", currentModel->getModelMatrix() );
        shaderAtlas[0]->setVec3("cameraPos_wS", sceneCamera->position);
        
        //Draw object
        currentModel->draw(*shaderAtlas[0]);
        renderObjectQueue->pop();
    }

}

void RenderManager::postProcess(const unsigned int start){
    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Shader setup for postprocessing
    shaderAtlas[1]->use();
    shaderAtlas[1]->setInt("offset", start);

    //Switching to the VAO of the quad and binding the texture buffer with
    // frame drawn off-screen
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void RenderManager::render(const unsigned int start){

    //First we draw the scene as normal but on the offscreen buffer
    drawScene();

    //Render to quad and apply postprocessing effects
    postProcess(start);

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

bool RenderManager::buildFrameBuffer(){
    int w = DisplayManager::SCREEN_WIDTH;
    int h = DisplayManager::SCREEN_HEIGHT;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //We generate and attach a texture to the frame buffer that acts as our color buffer
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR  );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR  );
    glBindTexture(GL_TEXTURE_2D, 0);

    //We now add the attachment to the framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //We generate a render buffer object for the stencil and depth buffer
    glGenRenderbuffers(1, &renderBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //Actually attaching to the frame buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);

    //Check if frame buffer is complete or not
    if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ){
        printf(" Failed to initialize the offscreen frame buffer!\n");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}










