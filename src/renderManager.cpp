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
    //something to push
    sceneCamera  = nullptr;
    sceneLocator = nullptr;
    screen = nullptr;
}

//Todo:: cleanup this mess
bool RenderManager::initFBOs(){
    numLights = currentScene->getLightCount();
    pointLightShadowFBOs = new DepthBuffer[numLights];
    unsigned int shadowMapResolution = currentScene->getShadowRes();

    bool initFBOFlagPointLights = true;
    for(unsigned int i = 0; i < 4; ++i ){
        initFBOFlagPointLights = initFBOFlagPointLights && pointLightShadowFBOs[i].setupFrameBuffer(shadowMapResolution, shadowMapResolution, true);
    }

    bool initFBOFlag1 = gBuffer.setupFrameBuffer();
    bool initFBOFLag2 = dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution, false);
    bool initFBOFlag3 = pingPong1.setupFrameBuffer();
    bool initFBOFlag4 = pingPong2.setupFrameBuffer();
    bool initFBOFlag5 = lightingBuffer.setupFrameBuffer();

    return true;
    // return initFBOFlag1 && initFBOFLag2 && initFBOFlag3 && initFBOFlag4 && initFBOFlagPointLights;
}

void RenderManager::render(const unsigned int start){
    //Initiating rendering gui
    ImGui::Begin("Rendering Controls");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    //Making sure depth testing is enabled 
    glEnable(GL_DEPTH_TEST);
    
    //Preps all the items that will be drawn in the scene
    buildRenderQueue();

    //Shadow mapping
    ImGui::Checkbox("Dynamic shadow Mapping", &hasMoved);
    if(hasMoved){
        //Populating depth cube maps for the point lights
        for (unsigned int i = 0; i < 4; ++i){
            pointLightShadowFBOs[i].bind();
            currentScene->drawPointLightShadow(shaderAtlas[3], i, pointLightShadowFBOs[i].depthMap);
        }

        // Directional shadows
        dirShadowFBO.bind();
        currentScene->drawDirLightShadows(shaderAtlas[2], dirShadowFBO.depthMap);
    }

    //Deffered rendering 
    //Filling the geometry buffer
    gBuffer.bind();
    currentScene->drawGeometry(shaderAtlas[0]);

    //Lighting pass
    lightingBuffer.bind();
    currentScene->setupLightingShader(shaderAtlas[1]);
    canvas.drawDeffered(gBuffer.positionBuffer, gBuffer.normalsBuffer, gBuffer.albedoSpecBuffer);

    //Applying post processing step
    postProcess(start);

    //Rendering gui scope ends here cannot be done later because the whole frame
    //is reset in the display buffer swap
    ImGui::End();

    //Drawing to the screen by swapping the window's surface with the
    //final buffer containing all rendering information
    screen->swapDisplayBuffer();
}

bool RenderManager::loadShaders(){
    shaderAtlas[0] = new Shader("gBufferShader.vert", "gBufferShader.frag");
    shaderAtlas[1] = new Shader("lightingShader.vert", "lightingShader.frag");
    shaderAtlas[2] = new Shader("shadowShader.vert", "shadowShader.frag");
    shaderAtlas[3] = new Shader("pointShadowShader.vert", "pointShadowShader.frag", "pointShadowShader.geom");
    shaderAtlas[4] = new Shader("splitHighShader.vert", "splitHighShader.frag");
    shaderAtlas[5] = new Shader("blurShader.vert", "blurShader.frag");
    shaderAtlas[6] = new Shader("screenShader.vert", "screenShader.frag");
    // shaderAtlas[5] = new Shader("basicShader.vert", "basicShader.frag");
    // shaderAtlas[2] = new Shader("skyboxShader.vert", "skyboxShader.frag");

    return true;
    // return ( shaderAtlas[0] != nullptr ) && ( shaderAtlas[1] != nullptr ) && ( shaderAtlas[2] != nullptr );
}

void RenderManager::postProcess(const unsigned int start){
    if(ImGui::CollapsingHeader("Post-processing")){
        ImGui::SliderInt("Blur", &sceneCamera->blurAmount, 0, 10);
        ImGui::SliderFloat("Exposure", &sceneCamera->exposure, 0.1f, 5.0f);
    }
    // Separating the high exposure content to the pingpongbuffer
    pingPong1.bind();
    shaderAtlas[4]->use();
    canvas.draw(lightingBuffer.texColorBuffer);

    //Applying Gaussian blur in ping pong fashion
    shaderAtlas[5]->use();

    for(int i =0; i < sceneCamera->blurAmount; ++i){
        //Horizontal pass 
        glBindFramebuffer(GL_FRAMEBUFFER, pingPong2.frameBufferID);
        // glDrawBuffer(GL_COLOR_ATTACHMENT1);
        shaderAtlas[5]->setBool("horizontal", true);
        canvas.draw(pingPong1.texColorBuffer);

        //Vertical pass 
        glBindFramebuffer(GL_FRAMEBUFFER, pingPong1.frameBufferID);
        shaderAtlas[5]->setBool("horizontal", false);
        canvas.draw(pingPong2.texColorBuffer);
    }

    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    screen->bind();

    //Shader setup for postprocessing
    shaderAtlas[6]->use();
    shaderAtlas[6]->setInt("offset", start);
    shaderAtlas[6]->setFloat("exposure", sceneCamera->exposure);
    shaderAtlas[6]->setInt("screenTexture", 0);
    shaderAtlas[6]->setInt("bloomBlur", 1);

    // //Convoluting both images
    canvas.draw(pingPong2.texColorBuffer, lightingBuffer.texColorBuffer);
}

//Done every frame in case scene changes
void RenderManager::buildRenderQueue(){
    //set scene info
    currentScene = sceneLocator->getCurrentScene();
    
    //Set renderer camera
    sceneCamera = currentScene->getCurrentCamera();
}
