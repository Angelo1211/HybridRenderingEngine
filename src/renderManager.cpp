// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-13
// ===============================

//Includes
#include "renderManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "debugUtils.h"

//Dummy constructors / Destructors
RenderManager::RenderManager(){}
RenderManager::~RenderManager(){}

//Sets the internal pointers to the screen and the current scene and inits the software
//renderer instance. 
bool RenderManager::startUp(DisplayManager &displayManager, SceneManager &sceneManager ){
    screen = &displayManager;
    sceneLocator = &sceneManager;
    currentScene = sceneLocator->getCurrentScene();

    if( !initFBOs() ){
        printf("One of the FBO's failed to be initialized correctly.\n");
        return false;
    }
    else{
        if (!loadShaders()){
            printf("A Shader failed to compile! \n");
            return false;
        }
        else{
            canvas.setupQuad();
        }
    }
    return true;
}

void RenderManager::shutDown(){
    delete shaderAtlas[0];
    delete shaderAtlas[1];
    delete shaderAtlas[2];
    delete shaderAtlas[3];
    delete shaderAtlas[4];
    delete shaderAtlas[5];
    delete shaderAtlas[6];

    sceneCamera  = nullptr;
    sceneLocator = nullptr;
    screen = nullptr;
}

bool RenderManager::initFBOs(){
    numLights = currentScene->getLightCount();
    pointLightShadowFBOs = new DepthBuffer[numLights];
    unsigned int shadowMapResolution = currentScene->getShadowRes();

    bool initFBOFlag1 = multiSampledFBO.setupFrameBuffer(true);
    bool initFBOFlag2 = simpleFBO.setupFrameBuffer();
    bool initFBOFLag3 = dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution, false);
    bool initFBOFlag4 = pingPongFBO.setupFrameBuffer();
    
    bool initFBOFlagPointLights = true;
    for(unsigned int i = 0; i < 4; ++i ){
        initFBOFlagPointLights = initFBOFlagPointLights && pointLightShadowFBOs[i].setupFrameBuffer(shadowMapResolution, shadowMapResolution, true);
    }

    return initFBOFlag1 && initFBOFlag2 && initFBOFLag3 && initFBOFlag4 && initFBOFlagPointLights;
}

void RenderManager::render(const unsigned int start){
    //Initiating rendering gui
    ImGui::Begin("Rendering Controls");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glEnable(GL_DEPTH_TEST);
    
    //Preps all the items that will be drawn in the scene
    buildRenderQueue();

    if(hasMoved){
        hasMoved = false;

        //Populating depth cube maps for the point lights
        for (unsigned int i = 0; i < 4; ++i){
            pointLightShadowFBOs[i].bind();
            currentScene->drawPointLightShadow(shaderAtlas[4], i, pointLightShadowFBOs[i].depthMap);
            // drawPointShadow(i);
        }

        // Directional shadows
        dirShadowFBO.bind();
        currentScene->drawDirLightShadows(shaderAtlas[3], dirShadowFBO.depthMap);
    }
    //Set the multisampled FBO as the first render target
    multiSampledFBO.bind();

    //First we draw the scene as normal but on the offscreen buffer
    currentScene->drawFullScene(shaderAtlas[0], shaderAtlas[2]);

    //Resolving the multisampled buffer into a regular one for postprocessing
    multiSampledFBO.blitTo(simpleFBO);

    //Render to quad and apply postprocessing effects
    postProcess(start);

    //Rendering gui scope ends here cannot be done later because the whole frame
    //is reset in the display buffer swap
    ImGui::End();

    //Drawing to the screen by swapping the window's surface with the
    //final buffer containing all rendering information
    screen->swapDisplayBuffer();
}

// void RenderManager::drawPointShadow(int i){

    //Setup matrices and shader
    // float zNear = 1.0f;
    // float zFar = 2000.0f;
    // float const ang = glm::radians(90.0f);
    // glm::mat4 shadowProj = glm::perspective(ang, aspect, zNear, zFar);

    //look at matrix setup
    // glm::mat4 lookAtPerFace[6];
    // glm::vec3 lightPos = pointLightPositions[i];
    //Shader setup
    // shaderAtlas[4]->use();
    // shaderAtlas[4]->setVec3("lightPos", lightPos);
    // shaderAtlas[4]->setFloat("far_plane", zFar);

    // for (unsigned int i = 0; i < 6; ++i){
    //     std::string number = std::to_string(i);
    //     lookAtPerFace[i] = shadowProj * lookAtPerFace[i];
    //     shaderAtlas[4]->setMat4(("shadowMatrices[" + number + "]").c_str(), lookAtPerFace[i]);
    // }

//     glm::mat4 M = glm::mat4(1.0);
//     for (unsigned int i = 0; i < renderObjectQueue->size(); ++i){
//         Model *currentModel = (*renderObjectQueue)[i];

//         M = currentModel->getModelMatrix();

//         //Shader setup stuff that changes every frame
//         shaderAtlas[4]->setMat4("M", M);

//         //Draw object
//         currentModel->draw(*shaderAtlas[4], false);
//     }
// }

bool RenderManager::loadShaders(){
    shaderAtlas[0] = new Shader("basicShader.vert", "basicShader.frag");
    shaderAtlas[1] = new Shader("screenShader.vert", "screenShader.frag");
    shaderAtlas[2] = new Shader("skyboxShader.vert", "skyboxShader.frag");
    shaderAtlas[3] = new Shader("shadowShader.vert", "shadowShader.frag");
    shaderAtlas[4] = new Shader("pointShadowShader.vert", "pointShadowShader.frag", "pointShadowShader.geom");
    shaderAtlas[5] = new Shader("splitHighShader.vert", "splitHighShader.frag");
    shaderAtlas[6] = new Shader("blurShader.vert", "blurShader.frag");

    return ( shaderAtlas[0] != nullptr ) && ( shaderAtlas[1] != nullptr ) && ( shaderAtlas[2] != nullptr );
}

//Here we do the offscreen rendering for hte whole scene
// void RenderManager::drawScene(){

    // //Temp matrix init TODO TODO TODO
    // glm::mat4 MVP = glm::mat4(1.0);
    // glm::mat4 M   = glm::mat4(1.0);
    // glm::mat4 VP  = sceneCamera->projectionMatrix * sceneCamera->viewMatrix;
    // glm::mat4 VPCubeMap = sceneCamera->projectionMatrix *glm::mat4(glm::mat3(sceneCamera->viewMatrix));

    //Activating shader and setting up uniforms that are constant
    // shaderAtlas[0]->use();
    
    //Directional light
    // ImGui::SliderFloat("Sun Strength", &dirLight.strength, 0.1f, 200.0f);
    // shaderAtlas[0]->setVec3("dirLight.direction", dirLight.direction);
    // shaderAtlas[0]->setVec3("dirLight.ambient",   dirLight.ambient);
    // shaderAtlas[0]->setVec3("dirLight.diffuse",   dirLight.strength * dirLight.color);
    // shaderAtlas[0]->setVec3("dirLight.specular",  dirLight.specular);

    // for(unsigned int i = 0; i < 4; ++i){
    //     std::string number = std::to_string(i);

    //     shaderAtlas[0]->setVec3(("pointLights["  + number + "].position").c_str(), pointLightPositions[i]);
    //     shaderAtlas[0]->setVec3(("pointLights["  + number + "].ambient").c_str(), glm::vec3(0.05f));
    //     shaderAtlas[0]->setVec3(("pointLights["  + number + "].diffuse").c_str(), pointLightColor[i]);
    //     shaderAtlas[0]->setVec3(("pointLights["  + number + "].specular").c_str(), glm::vec3(0.4f));
    //     shaderAtlas[0]->setFloat(("pointLights[" + number + "].constant").c_str(), 1.0f);
    //     shaderAtlas[0]->setFloat(("pointLights[" + number + "].linear").c_str(), 0.007f);
    //     shaderAtlas[0]->setFloat(("pointLights[" + number + "].quadratic").c_str(), 0.0002f);

    //     glActiveTexture(GL_TEXTURE2 + i);
    //     shaderAtlas[0]->setInt(("pointLights[" + number + "].depthMap").c_str(), 2 + i);
    //     glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightShadowFBOs[i].depthMap);
    //     shaderAtlas[0]->setFloat("far_plane", 2000.0f);
    // }

    // for(unsigned int i = 0; i < renderObjectQueue->size(); ++i){
    //     Model * currentModel = (*renderObjectQueue)[i];

        //Light movement
        // float ang = 2.0f* M_PI * static_cast<float>(SDL_GetTicks()) / (16000.0f);
        // float radius = 1000.0f;
        // float X = std::sin(ang) * radius;
        // float Z = std::cos(ang) * radius;
        // glm::vec3 lightPos = glm::vec3(X, 100.0f , 0.0f);

    //     //Matrix setup
    //     M  = currentModel->getModelMatrix();
    //     MVP = VP * M;

    //     //Shader setup stuff that changes every frame
    //     shaderAtlas[0]->setMat4("MVP", MVP);
    //     shaderAtlas[0]->setMat4("M", M);
    //     shaderAtlas[0]->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    //     shaderAtlas[0]->setVec3("cameraPos_wS", sceneCamera->position);

    //     glActiveTexture(GL_TEXTURE6);
    //     shaderAtlas[0]->setInt("shadowMap", 6);
    //     glBindTexture(GL_TEXTURE_2D, dirShadowFBO.depthMap);
        
    //     //Draw object
    //     currentModel->draw(*shaderAtlas[0], true);
    // }

    // //Drawing skybox
    // shaderAtlas[2]->use();
    // shaderAtlas[2]->setMat4("VP", VPCubeMap);
    // skybox->draw();
// }

void RenderManager::postProcess(const unsigned int start){
    // Separating the high exposure content to the pingpongbuffer
    pingPongFBO.bind();
    shaderAtlas[5]->use();
    canvas.draw(simpleFBO.texColorBuffer);

    //Applying Gaussian blur in ping pong fashion
    shaderAtlas[6]->use();

    ImGui::SliderInt("Blur", &amount, 0, 10);
    for(int i =0; i < amount; ++i){
        //Horizontal pass 
        glBindFramebuffer(GL_FRAMEBUFFER, simpleFBO.frameBufferID);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        shaderAtlas[6]->setBool("horizontal", true);
        canvas.draw(pingPongFBO.texColorBuffer);

        //Vertical pass 
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO.frameBufferID);
        shaderAtlas[6]->setBool("horizontal", false);
        canvas.draw(simpleFBO.blurHighEnd);
    }

    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    screen->bind();

    ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
    //Shader setup for postprocessing
    shaderAtlas[1]->use();
    shaderAtlas[1]->setInt("offset", start);
    shaderAtlas[1]->setFloat("exposure", exposure);
    shaderAtlas[1]->setInt("screenTexture", 0);
    shaderAtlas[1]->setInt("bloomBlur", 1);

    //Convoluting both images
    canvas.draw(pingPongFBO.texColorBuffer, simpleFBO.texColorBuffer);
}

//Gets the list of visible models from the current scene
//Done every frame in case scene changes
void RenderManager::buildRenderQueue(){

    //set scene info
    currentScene = sceneLocator->getCurrentScene();
    
    //Set renderer camera
    sceneCamera = currentScene->getCurrentCamera();
    
    //Update the pointer to the list of lights in the scene
    // dirLight = currentScene->getDirectionalLight();
    // renderInstance.setSceneLights(currentScene->getCurrentLights(), currentScene->getLightCount() );

    //Get pointers to the visible model queue
    // renderObjectQueue = currentScene->getVisiblemodels();
}
