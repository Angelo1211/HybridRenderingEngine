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
    int scale =  1;
    bool initFBOFlag1 = multiSampledFBO.setupFrameBuffer(true);
    bool initFBOFlag2 = simpleFBO.setupFrameBuffer();
    bool initFBOFLag3 = shadowFBO.setupFrameBuffer(1024, 1024);
    if( !(initFBOFlag1 && initFBOFlag2 && initFBOFLag3) ){
        return false;
    }
    else{
        if (!loadShaders()){
            printf("A Shader failed to compile! \n");
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
    delete shaderAtlas[2];
    delete shaderAtlas[3];
    // delete [] shaderAtlas;
    sceneCamera  = nullptr;
    sceneLocator = nullptr;
    screen = nullptr;
}

void RenderManager::render(const unsigned int start){
    //Set shadow fbo to draw all our shadow stuff to the depth buffer
    shadowFBO.bind();

    //Drawing to the depth buffer
    //TO DO FIX FIX FIX FIX FIX FIX FIX
    //You are making the renderQueue twice, which is wasteful and bad. 
    //But I think it works for now.
    buildRenderQueue();

    drawSceneFromLightPOV();

    //Set the multisampled FBO as the first render target
    multiSampledFBO.bind();

    //Preps all the items that will be drawn in the scene
    // buildRenderQueue();

    //First we draw the scene as normal but on the offscreen buffer
    drawScene();

    //Resolving the multisampled buffer into a regular one for postprocessing
    multiSampledFBO.blitTo(simpleFBO);

    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glViewport(0,0,DisplayManager::SCREEN_WIDTH, DisplayManager::SCREEN_HEIGHT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Render to quad and apply postprocessing effects
    postProcess(start);

    //Drawing to the screen by swapping the window's surface with the
    //final buffer containing all rendering information
    screen->swapDisplayBuffer();

    //Set camera pointer to null just in case a scene change occurs
    sceneCamera = nullptr;
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
    shaderAtlas[2] = new Shader("skyboxShader.vert", "skyboxShader.frag");
    shaderAtlas[3] = new Shader("shadowShader.vert", "shadowShader.frag");


    // shaderAtlas[2]->use();
    // shaderAtlas[2]->setInt("skybox", 0);

    return ( shaderAtlas[0] != nullptr ) && ( shaderAtlas[1] != nullptr ) && ( shaderAtlas[2] != nullptr );
}

void RenderManager::drawSceneFromLightPOV(){
    glEnable(GL_DEPTH_TEST);
    float left   = 3000.0f;
    float right  = -left;
    float top    = left;
    float bottom = -top;
    float zNear  = 1.0f;
    float zFar   = 7000.0f;
    glm::mat4 lightProjection = glm::ortho(left, right, bottom, top, zNear, zFar);
    
    glm::mat4 lightView = glm::lookAt(1500.0f * dirLightPosition,
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    lightSpaceMatrix = lightProjection * lightView;
    glm::mat4 ModelLS = glm::mat4(1.0);


    for(unsigned int i = 0; i < renderObjectQueue->size(); ++i){
        Model * currentModel = (*renderObjectQueue)[i];

        //Matrix setup
        ModelLS = lightSpaceMatrix * currentModel->getModelMatrix();

        //Shader setup stuff that changes every frame
        shaderAtlas[3]->use();
        shaderAtlas[3]->setMat4("lightSpaceMatrix", ModelLS);
        
        //Draw object
        currentModel->draw(*shaderAtlas[3]);
    }

}

//Here we do the offscreen rendering for hte whole scene
void RenderManager::drawScene(){


    //Temp matrix init TODO TODO TODO
    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 M   = glm::mat4(1.0);
    glm::mat4 VP  = sceneCamera->projectionMatrix * sceneCamera->viewMatrix;
    glm::mat4 VPCubeMap = sceneCamera->projectionMatrix *glm::mat4(glm::mat3(sceneCamera->viewMatrix));


    //Activating shader and setting up uniforms that are constant
    shaderAtlas[0]->use();
    
    //Directional light
    shaderAtlas[0]->setVec3("dirLight.direction", -dirLightPosition);
    shaderAtlas[0]->setVec3("dirLight.ambient",   glm::vec3(0.02f));
    shaderAtlas[0]->setVec3("dirLight.diffuse",   glm::vec3(0.9f));
    shaderAtlas[0]->setVec3("dirLight.specular",  glm::vec3(0.5f));


    glm::vec3 pointLightColor[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 1.0f)
    };
    for(unsigned int i = 0; i < 4; ++i){
        std::string number = std::to_string(i);

        shaderAtlas[0]->setVec3(("pointLights["  + number + "].position").c_str(), pointLightPositions[i]);
        shaderAtlas[0]->setVec3(("pointLights["  + number + "].ambient").c_str(), glm::vec3(0.05f));
        shaderAtlas[0]->setVec3(("pointLights["  + number + "].diffuse").c_str(), pointLightColor[i]);
        shaderAtlas[0]->setVec3(("pointLights["  + number + "].specular").c_str(), glm::vec3(0.4f));
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].constant").c_str(), 1.0f);
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].linear").c_str(), 0.007f);
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].quadratic").c_str(), 0.0002f);
    }

    // while( !renderObjectQueue->empty() ){
    for(unsigned int i = 0; i < renderObjectQueue->size(); ++i){
        Model * currentModel = (*renderObjectQueue)[i];

        //Light movement
        // float ang = 2.0f* M_PI * static_cast<float>(SDL_GetTicks()) / (16000.0f);
        // float radius = 1000.0f;
        // float X = std::sin(ang) * radius;
        // float Z = std::cos(ang) * radius;
        // glm::vec3 lightPos = glm::vec3(X, 100.0f , 0.0f);

        //Matrix setup
        M  = currentModel->getModelMatrix();
        MVP = VP * M;

        //Shader setup stuff that changes every frame
        shaderAtlas[0]->setMat4("MVP", MVP);
        shaderAtlas[0]->setMat4("M", M);
        shaderAtlas[0]->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shaderAtlas[0]->setVec3("cameraPos_wS", sceneCamera->position);

        glActiveTexture(GL_TEXTURE2);
        shaderAtlas[0]->setInt("shadowMap", 2);
        glBindTexture(GL_TEXTURE_2D, shadowFBO.depthMap);
        
        //Draw object
        currentModel->draw(*shaderAtlas[0]);
        // renderObjectQueue->pop_back();
    }

    //Drawing skybox
    drawSkybox(VPCubeMap);
}

void RenderManager::drawSkybox(const glm::mat4  &VP){
    //We change the depth function because we set the skybox to always have
    // a clipspace value of one so if it isn't changed to less than or equal it will fail
    glDepthFunc(GL_LEQUAL);
    shaderAtlas[2]->use();
    shaderAtlas[2]->setMat4("VP", VP);

    skybox->draw();
    glDepthFunc(GL_LESS);
}

void RenderManager::postProcess(const unsigned int start){
    //Shader setup for postprocessing
    shaderAtlas[1]->use();
    shaderAtlas[1]->setInt("offset", start);

    //Switching to the VAO of the quad and binding the texture buffer with
    // frame drawn off-screen
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    // glBindTexture(GL_TEXTURE_2D, shadowFBO.depthMap);
    glBindTexture(GL_TEXTURE_2D, simpleFBO.texColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glDisable(GL_FRAMEBUFFER_SRGB);

    glBindVertexArray(0);
}


//Gets the list of visible models from the current scene
//Done every frame in case scene changes
void RenderManager::buildRenderQueue(){

    //set scene info
    Scene* currentScene = sceneLocator->getCurrentScene();
    
    //Set renderer camera
    sceneCamera = currentScene->getCurrentCamera();
    
    skybox = currentScene->getCurrentSkybox();

    //Update the pointer to the list of lights in the scene
    // renderInstance.setSceneLights(currentScene->getCurrentLights(), currentScene->getLightCount() );

    //Get pointers to the visible model queue
    renderObjectQueue = currentScene->getVisiblemodels();
}