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

struct TileFrustrum{
    //Contains the normal as xyz and the D constant as W
    //Follows the following convention:
    // enum planes{
    // TOP = 0, BOTTOM, LEFT,
    // RIGHT, NEARP, FARP};
    glm::vec4 plane[4];
} frustrum;

struct ScreenToView{
    glm::mat4 inverseProjectionMat;
    glm::vec4 screenDimensions;
}screen2View;

//Sets the internal pointers to the screen and the current scene and inits the software
//renderer instance. 
bool RenderManager::startUp(DisplayManager &displayManager, SceneManager &sceneManager ){
    printf("Here!\n");
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
            //Setup of other important rendering items
            //TODO:: not everything should be here
            buildRenderQueue();
            canvas.setupQuad();
            initSSBOs();

        }
    }
    return true;
}

bool RenderManager::initSSBOs(){
    //Setting up tile size
    size = 80;
    tileNumX = DisplayManager::SCREEN_WIDTH / size;
    tileNumY = DisplayManager::SCREEN_HEIGHT / size;
    numTiles = tileNumX * tileNumY;
    
    //Setting up the frustrum SSBO
    glGenBuffers(1, &frustrumSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, frustrumSSBO);

    //We generate the buffer but don't populate it yet.
    //In fact we don't populate it at all!
    //We're okay with the initialized values
    glBufferData(GL_SHADER_STORAGE_BUFFER, numTiles * sizeof(struct TileFrustrum), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, frustrumSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //Setting up screen2View ssbo
    glGenBuffers(1, &screenToViewSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, screenToViewSSBO);

    //Setting up contents of buffer
    screen2View.inverseProjectionMat = glm::inverse(sceneCamera->projectionMatrix);
    screen2View.screenDimensions = glm::vec4(DisplayManager::SCREEN_WIDTH, DisplayManager::SCREEN_HEIGHT, 0.0, 0.0);

    //Generating and copying data to memory in GPU
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView), &screen2View, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, screenToViewSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return true;
}

bool RenderManager::loadShaders(){
    shaderAtlas[0] = new Shader("depthPassShader.vert", "depthPassShader.frag");
    shaderAtlas[1] = new Shader("basicShader.vert", "basicShader.frag");
    shaderAtlas[2] = new Shader("skyboxShader.vert", "skyboxShader.frag");
    shaderAtlas[3] = new Shader("splitHighShader.vert", "splitHighShader.frag");
    shaderAtlas[4] = new Shader("blurShader.vert", "blurShader.frag");
    shaderAtlas[5] = new Shader("screenShader.vert", "screenShader.frag");
    shaderAtlas[6] = new Shader("shadowShader.vert", "shadowShader.frag");
    shaderAtlas[7] = new Shader("pointShadowShader.vert", "pointShadowShader.frag", "pointShadowShader.geom");

    // shaderAtlas[1] = new Shader("testShader.vert", "testShader.frag");
    // shaderAtlas[5] = new Shader("lightingShader.vert", "lightingShader.frag");
    // shaderAtlas[0] = new Shader("gBufferShader.vert", "gBufferShader.frag");

    computeFrustrumPerTile = new ComputeShader("frustrumShader.comp");

    return true;
    // return ( shaderAtlas[0] != nullptr ) && ( shaderAtlas[1] != nullptr ) && ( shaderAtlas[2] != nullptr );
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

//Todo:: cleanup this mess
bool RenderManager::initFBOs(){
    numLights = currentScene->getLightCount();
    pointLightShadowFBOs = new DepthBuffer[numLights];
    unsigned int shadowMapResolution = currentScene->getShadowRes();

    bool initFBOFlag1 = multiSampledFBO.setupFrameBuffer(true);
    bool initFBOFlag2 = pingPongFBO.setupFrameBuffer();
    bool initFBOFlag3 = simpleFBO.setupFrameBuffer();
    bool initFBOFLag4 = dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution, false);
    // bool initFBOFlag1 = depthPrePass.setupFrameBuffer(DisplayManager::SCREEN_WIDTH,
                                                    //   DisplayManager::SCREEN_HEIGHT,
                                                    //   false);

    bool initFBOFlagPointLights = true;
    for(unsigned int i = 0; i < numLights; ++i ){
        initFBOFlagPointLights = initFBOFlagPointLights && pointLightShadowFBOs[i].setupFrameBuffer(shadowMapResolution, shadowMapResolution, true);
    }

    // bool initFBOFlag1 = gBuffer.setupFrameBuffer();
    // bool initFBOFLag2 = dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution, false);
    // bool initFBOFlag3 = pingPong1.setupFrameBuffer();
    // bool initFBOFlag4 = pingPong2.setupFrameBuffer();
    // bool initFBOFlag5 = lightingBuffer.setupFrameBuffer();

    return true;
    // return initFBOFlag1 && initFBOFLag2 && initFBOFlag3 && initFBOFlag4 && initFBOFlagPointLights;
}

/* This time using forward+
Algorithm steps:
//Initialization or view frustrum change
0. Generate all sides of tile frustrum except near and far 
//Every frame
1. Depth-pre pass 
2. Compute shader to update light frustrums to get near and far values
3. Compute shader to cull lights
4. Actually perform shading as usual 
*/
void RenderManager::render(const unsigned int start){
    //Initiating rendering gui
    ImGui::Begin("Rendering Controls");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    //Making sure depth testing is enabled 
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    //Shadow mapping
    ImGui::Checkbox("Dynamic shadow Mapping", &hasMoved);
    if(hasMoved){
        //Populating depth cube maps for the point lights
        for (unsigned int i = 0; i < 4; ++i){
            pointLightShadowFBOs[i].bind();
            currentScene->drawPointLightShadow(shaderAtlas[7], i, pointLightShadowFBOs[i].depthMap);
        }

        // Directional shadows
        dirShadowFBO.bind();
        currentScene->drawDirLightShadows(shaderAtlas[6], dirShadowFBO.depthMap);
        hasMoved = false;
    }

    
    //Preps all the items that will be drawn in the scene
    buildRenderQueue();

    //1-Depth pre-pass
    multiSampledFBO.bind();
    currentScene->drawDepthPass(shaderAtlas[0]);

    //2, 3 - Light culling in compute shader currently a stub
    computeFrustrumPerTile->use();
    glDispatchCompute(tileNumX, tileNumY, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    //4 - Actual shading
    //4.1 - Forward render the scene in the multisampled FBO using the z buffer to discard early
    glDepthFunc(GL_EQUAL);
    glDepthMask(false);
    currentScene->drawFullScene(shaderAtlas[1], shaderAtlas[2]);

    //4.2 - resolve the zBuffer from multisampled to regular one using blitting for postprocessing
    multiSampledFBO.blitTo(simpleFBO);

    //4.3 -postprocess
    postProcess(start);


    //Drawing the final result of the compute shader to the screen to get an idea
    //of how well it worked
    // screen->bind();
    // shaderAtlas[1]->use();
    // glActiveTexture(GL_TEXTURE0);
    // shaderAtlas[1]->setInt("screenTexture", 0);
    // glBindTexture(GL_TEXTURE_2D, testTexture);
    // canvas.draw(testTexture);


    // //Deffered rendering 
    // //Filling the geometry buffer
    // gBuffer.bind();
    // currentScene->drawGeometry(shaderAtlas[0]);

    // //Lighting pass
    // lightingBuffer.bind();
    // currentScene->setupLightingShader(shaderAtlas[1]);
    // canvas.drawDeffered(gBuffer.positionBuffer, gBuffer.normalsBuffer, gBuffer.albedoSpecBuffer);

    //Rendering gui scope ends here cannot be done later because the whole frame
    //is reset in the display buffer swap
    ImGui::End();

    //Drawing to the screen by swapping the window's surface with the
    //final buffer containing all rendering information
    screen->swapDisplayBuffer();
}


void RenderManager::postProcess(const unsigned int start){
    if(ImGui::CollapsingHeader("Post-processing")){
        ImGui::SliderInt("Blur", &sceneCamera->blurAmount, 0, 10);
        ImGui::SliderFloat("Exposure", &sceneCamera->exposure, 0.1f, 5.0f);
    }
    pingPongFBO.bind();
    shaderAtlas[3]->use();
    canvas.draw(simpleFBO.texColorBuffer);
    //Applying Gaussian blur in ping pong fashion
    shaderAtlas[4]->use();
    for (int i = 0; i < sceneCamera->blurAmount; ++i)
    {
        //Horizontal pass
        glBindFramebuffer(GL_FRAMEBUFFER, simpleFBO.frameBufferID);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        shaderAtlas[4]->setBool("horizontal", true);
        canvas.draw(pingPongFBO.texColorBuffer);
        //Vertical pass
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO.frameBufferID);
        shaderAtlas[4]->setBool("horizontal", false);
        canvas.draw(simpleFBO.blurHighEnd);
    }
    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    screen->bind();

    //Shader setup for postprocessing
    shaderAtlas[5]->use();
    shaderAtlas[5]->setInt("offset", start);
    shaderAtlas[5]->setFloat("exposure", sceneCamera->exposure);
    shaderAtlas[5]->setInt("screenTexture", 0);
    shaderAtlas[5]->setInt("bloomBlur", 1);
    shaderAtlas[5]->setInt("computeTexture", 2);
    //Convoluting both images
    canvas.draw(pingPongFBO.texColorBuffer, simpleFBO.texColorBuffer);

    // Separating the high exposure content to the pingpongbuffer
    // pingPong1.bind();
    // shaderAtlas[4]->use();
    // canvas.draw(lightingBuffer.texColorBuffer);

    // //Applying Gaussian blur in ping pong fashion
    // shaderAtlas[5]->use();

    // for(int i =0; i < sceneCamera->blurAmount; ++i){
    //     //Horizontal pass 
    //     glBindFramebuffer(GL_FRAMEBUFFER, pingPong2.frameBufferID);
    //     // glDrawBuffer(GL_COLOR_ATTACHMENT1);
    //     shaderAtlas[5]->setBool("horizontal", true);
    //     canvas.draw(pingPong1.texColorBuffer);

    //     //Vertical pass 
    //     glBindFramebuffer(GL_FRAMEBUFFER, pingPong1.frameBufferID);
    //     shaderAtlas[5]->setBool("horizontal", false);
    //     canvas.draw(pingPong2.texColorBuffer);
    // }

    // //Setting back to default framebuffer (screen) and clearing
    // //No need for depth testing cause we're drawing to a flat quad
    // screen->bind();

    // //Shader setup for postprocessing
    // shaderAtlas[6]->use();
    // shaderAtlas[6]->setInt("offset", start);
    // shaderAtlas[6]->setFloat("exposure", sceneCamera->exposure);
    // shaderAtlas[6]->setInt("screenTexture", 0);
    // shaderAtlas[6]->setInt("bloomBlur", 1);

    // // //Convoluting both images
    // canvas.draw(pingPong2.texColorBuffer, lightingBuffer.texColorBuffer);
}

//Done every frame in case scene changes
void RenderManager::buildRenderQueue(){
    //set scene info
    currentScene = sceneLocator->getCurrentScene();
    
    //Set renderer camera
    sceneCamera = currentScene->getCurrentCamera();
}




// //Finding the max sizes for compute units
// int work_grp_cnt[3];

// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

// printf("max global (total) work group size x:%i y:%i z:%i\n",
//        work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

// int work_grp_size[3];

// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

// printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
//        work_grp_size[0], work_grp_size[1], work_grp_size[2]);
