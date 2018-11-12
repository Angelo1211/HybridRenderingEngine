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
#include "cmath"

//Dummy constructors / Destructors
RenderManager::RenderManager(){}
RenderManager::~RenderManager(){}

//Temporarily here will move them later
struct VolumeTileAABB{
    glm::vec4 minPoint;
    glm::vec4 maxPoint;
} frustrum;

struct ScreenToView{
    glm::mat4 inverseProjectionMat;

    unsigned int tileSizes[4];

    unsigned int screenWidth;
    unsigned int screenHeight;
    float sliceScalingFactor;
    float sliceBiasFactor;
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

            //TODO:: not everything should be here
            computeGridAABB->use();
            computeGridAABB->setFloat("zNear", sceneCamera->cameraFrustrum.nearPlane);
            computeGridAABB->setFloat("zFar", sceneCamera->cameraFrustrum.farPlane);
            glDispatchCompute(gridSizeX, gridSizeY, gridSizeZ);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            //Passing equirectangular map to cubemap
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBOBig.frameBufferID);
            currentScene->mainSkyBox.fillCubeMapWithTexture(shaderAtlas[8]);

            //Cubemap convolution TODO:: Fix ugly function call
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBOSmall.frameBufferID);
            unsigned int environmentID = currentScene->mainSkyBox.skyBoxCubeMap.textureID;
            unsigned int cubeVAO       = currentScene->mainSkyBox.VAO;
            currentScene->irradianceMap.convolveCubeMap(environmentID, cubeVAO, shaderAtlas[9]);

            glViewport(0, 0, DisplayManager::SCREEN_WIDTH, DisplayManager::SCREEN_HEIGHT);

            currentScene->mainSkyBox.skyBoxCubeMap.textureID = currentScene->irradianceMap.textureID;
        }
    }
    return true;
}

bool RenderManager::initSSBOs(){
    //Setting up tile size on both X and Y 
    sizeX =  (unsigned int)std::ceilf(DisplayManager::SCREEN_WIDTH / (float)gridSizeX);
    // sizeY =  (unsigned int)std::ceilf(DisplayManager::SCREEN_HEIGHT / (float)gridSizeY);

    float zFar    =  sceneCamera->cameraFrustrum.farPlane;
    float zNear   =  sceneCamera->cameraFrustrum.nearPlane;

    // tileNumZ = (unsigned int)std::floorf( num / denom);

    // cullDispatchX = (unsigned int)std::ceilf(tileNumX / (float)size);
    // cullDispatchY = (unsigned int)std::ceilf(tileNumY / (float)size);
    
    //Setting up the AABB volume grid
    {
        glGenBuffers(1, &volumeGridAABB_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeGridAABB_SSBO);

        //We generate the buffer but don't populate it yet.
        glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(struct VolumeTileAABB), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, volumeGridAABB_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Setting up screen2View ssbo
    {
        glGenBuffers(1, &screenToViewSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, screenToViewSSBO);

        //Setting up contents of buffer
        screen2View.inverseProjectionMat = glm::inverse(sceneCamera->projectionMatrix);
        screen2View.tileSizes[0] = gridSizeX;
        screen2View.tileSizes[1] = gridSizeY;
        screen2View.tileSizes[2] = gridSizeZ;
        screen2View.tileSizes[3] = sizeX;
        screen2View.screenWidth  = DisplayManager::SCREEN_WIDTH;
        screen2View.screenHeight = DisplayManager::SCREEN_HEIGHT;
        screen2View.sliceScalingFactor = (float)gridSizeZ / std::log2f(zFar / zNear) ;
        screen2View.sliceBiasFactor    = -((float)gridSizeZ * std::log2f(zNear) / std::log2f(zFar / zNear)) ;

        //Generating and copying data to memory in GPU
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView), &screen2View, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screenToViewSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Setting up lights buffer that contains all the lights in the scene
    {
        glGenBuffers(1, &lightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxLights * sizeof(struct GPULight), NULL, GL_DYNAMIC_DRAW);

        GLint bufMask = GL_READ_WRITE;

        struct GPULight *lights = (struct GPULight *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, bufMask);
        PointLight *light;
        for(unsigned int i = 0; i < numLights; ++i ){
            //Fetching the light from the current scene
            light = currentScene->getPointLight(i);
            lights[i].position  = glm::vec4(light->position, 1.0f);
            lights[i].color     = glm::vec4(light->color, 1.0f);
            lights[i].enabled   = 1; 
            lights[i].intensity = 1.0f;
            lights[i].range     = 65.0f;
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Setting up light index list
    {
        unsigned int totalNumLights =  numClusters * maxLightsPerTile; //100 lights per tile max
        glGenBuffers(1, &lightIndexList);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexList);

        //We generate the buffer but don't populate it yet.
        glBufferData(GL_SHADER_STORAGE_BUFFER,  totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexList);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Setting up light grid 
    {
        glGenBuffers(1, &lightGrid);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightGrid);

        //Every tile takes two unsigned ints one to represent the number of lights in that grid
        //Another to represent the offset 
        glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGrid);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Setting up simplest ssbo in the world
    {
        glGenBuffers(1, &lightIndexGlobalCount);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexGlobalCount);

        //Every tile takes two unsigned ints one to represent the number of lights in that grid
        //Another to represent the offset 
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightIndexGlobalCount);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
   
    return true;
}

bool RenderManager::loadShaders(){
    shaderAtlas[0] = new Shader("depthPassShader.vert", "depthPassShader.frag");
    shaderAtlas[1] = new Shader("PBRClusteredShader.vert", "PBRClusteredShader.frag");
    shaderAtlas[2] = new Shader("skyboxShader.vert", "skyboxShader.frag");
    shaderAtlas[3] = new Shader("splitHighShader.vert", "splitHighShader.frag");
    shaderAtlas[4] = new Shader("blurShader.vert", "blurShader.frag");
    shaderAtlas[5] = new Shader("screenShader.vert", "screenShader.frag");
    shaderAtlas[6] = new Shader("shadowShader.vert", "shadowShader.frag");
    shaderAtlas[7] = new Shader("pointShadowShader.vert", "pointShadowShader.frag", "pointShadowShader.geom");
    shaderAtlas[8] = new Shader("cubeMapShader.vert", "buildCubeMapShader.frag");
    shaderAtlas[9] = new Shader("cubeMapShader.vert", "convolveCubemapShader.frag");

    computeGridAABB = new ComputeShader("clusterShader.comp");
    cullLightsAABB  = new ComputeShader("clusterCullLightShader.comp");
    // computeDepths = new ComputeShader("depthBoundShader.comp");

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
//Fix capture FBO name convention
bool RenderManager::initFBOs(){
    numLights = currentScene->getLightCount();
    pointLightShadowFBOs = new DepthBuffer[numLights];
    unsigned int shadowMapResolution = currentScene->getShadowRes();

    bool initFBOFlag1 = multiSampledFBO.setupFrameBuffer(true);
    bool initFBOFlag2 = pingPongFBO.setupFrameBuffer();
    bool initFBOFlag3 = simpleFBO.setupFrameBuffer();
    bool initFBOFLag4 = dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution, false);

    int skyboxRes = currentScene->mainSkyBox.resolution;
    captureFBOBig.setupFrameBuffer(skyboxRes, skyboxRes);

    int irradianceRes = currentScene->irradianceMap.width;
    captureFBOSmall.setupFrameBuffer(irradianceRes, irradianceRes);
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

/* This time using volume tiled forward
Algorithm steps:
//Initialization or view frustrum change
0. Determine AABB's for each volume 
//Update Every frame
1. Depth-pre pass :: DONE
2. Mark Active tiles :: POSTPONED AS OPTIMIZATION
3. Build Tile list ::  POSTPONED AS OPTIMIZATION
4. Assign lights to tiles 
5. Actually perform shading as usual 
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
    }

    //Preps all the items that will be drawn in the scene
    buildRenderQueue();

    //1.1- Multisampled Depth pre-pass
    multiSampledFBO.bind();
    currentScene->drawDepthPass(shaderAtlas[0]);

    //1.2- Multisampled blitting to depth texture
    multiSampledFBO.blitTo(simpleFBO, GL_DEPTH_BUFFER_BIT);

    // //2.2- Near and far plane update
    // computeDepths->use();
    // computeDepths->setFloat("zNear", sceneCamera->cameraFrustrum.nearPlane);
    // computeDepths->setFloat("zFar", sceneCamera->cameraFrustrum.farPlane);

    // glBindImageTexture(0, simpleFBO.depthBuffer, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, simpleFBO.depthBuffer);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    // computeDepths->setInt("depthMap", 0);

    // glDispatchCompute(tileNumX, tileNumY, 1);
    // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //4-Light culling
    cullLightsAABB->use();
    cullLightsAABB->setMat4("viewMatrix", sceneCamera->viewMatrix);
    glDispatchCompute(1, 1, 6);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //4 - Actual shading
    //4.1 - Forward render the scene in the multisampled FBO using the z buffer to discard early
    glDepthFunc(GL_EQUAL);
    // glDepthFunc(GL_EQUAL);
    glDepthMask(false);
    currentScene->drawFullScene(shaderAtlas[1], shaderAtlas[2]);

    //4.2 - resolve the zBuffer from multisampled to regular one using blitting for postprocessing
    multiSampledFBO.blitTo(simpleFBO, GL_COLOR_BUFFER_BIT);

    //4.3 -postprocess
    postProcess(start);

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
    if( sceneCamera->blurAmount > 0){
        shaderAtlas[3]->use();
        canvas.draw(simpleFBO.texColorBuffer);
    }
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
    canvas.draw(simpleFBO.texColorBuffer, pingPongFBO.texColorBuffer);

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
