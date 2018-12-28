/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-13
*/

//Includes
#include "renderManager.h"
#include "imgui/imgui.h"
#include "debugUtils.h"
#include "gpuData.h"
#include "cmath"

//Dummy constructors / Destructors
RenderManager::RenderManager(){}
RenderManager::~RenderManager(){}

//Sets the internal pointers to the screen and the current scene and inits the software
//renderer instance. 
bool RenderManager::startUp(DisplayManager &displayManager, SceneManager &sceneManager ){
    printf("\nInitializing Renderer.\n");
    //Getting pointers to the data we'll render
    screen = &displayManager;
    sceneLocator = &sceneManager;
    currentScene = sceneLocator->getCurrentScene();
    sceneCamera = currentScene->getCurrentCamera();

    printf("Loading FBO's...\n");
    if( !initFBOs() ){
        printf("FBO's failed to be initialized correctly.\n");
        return false;
    }
 
    printf("Loading Shaders...\n");
    if (!loadShaders()){
        printf("Shaders failed to be initialized correctly.\n");
        return false;
    }

    printf("Loading SSBO's...\n");
    if (!initSSBOs()){
        printf("SSBO's failed to be initialized correctly.\n");
        return false;
    }

    printf("Preprocessing...\n");
    if (!preProcess()){
        printf("SSBO's failed to be initialized correctly.\n");
        return false;
    }

    printf("Renderer Initialization complete.\n");
    return true;
}

bool RenderManager::preProcess(){
    //Initializing the surface that we use to draw screen-space effects
    canvas.setup();

    //Building the grid of AABB enclosing the view frustum clusters
    buildAABBGridCompShader.use();
    buildAABBGridCompShader.setFloat("zNear", sceneCamera->cameraFrustum.nearPlane);
    buildAABBGridCompShader.setFloat("zFar", sceneCamera->cameraFrustum.farPlane);
    buildAABBGridCompShader.dispatch(gridSizeX, gridSizeY, gridSizeZ);

    //Environment Mapping
    //Passing equirectangular map to cubemap
    captureFBO.bind();
    currentScene->mainSkyBox.fillCubeMapWithTexture(fillCubeMapShader);

    //Cubemap convolution TODO:: This could probably be moved to a function of the scene or environment maps
    //themselves as a class / static function
    int res = currentScene->irradianceMap.width;
    captureFBO.resizeFrameBuffer(res);
    unsigned int environmentID = currentScene->mainSkyBox.skyBoxCubeMap.textureID;
    currentScene->irradianceMap.convolveCubeMap(environmentID, convolveCubeMap);

    //Cubemap prefiltering TODO:: Same as above
    unsigned int captureRBO = captureFBO.depthBuffer;
    currentScene->specFilteredMap.preFilterCubeMap(environmentID, captureRBO, preFilterSpecShader);

    //BRDF lookup texture
    integrateBRDFShader.use();
    res = currentScene->brdfLUTTexture.height;
    captureFBO.resizeFrameBuffer(res);
    unsigned int id = currentScene->brdfLUTTexture.textureID;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
    glViewport(0, 0, res, res);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    canvas.draw();

    //Making sure that the viewport is the correct size after rendering
    glViewport(0, 0, DisplayManager::SCREEN_WIDTH, DisplayManager::SCREEN_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    //Populating depth cube maps for the point light shadows
    for (unsigned int i = 0; i < currentScene->pointLightCount; ++i){
        pointLightShadowFBOs[i].bind();
        pointLightShadowFBOs[i].clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
        currentScene->drawPointLightShadow(pointShadowShader,i, pointLightShadowFBOs[i].depthBuffer);
    }

    // Directional shadows
    dirShadowFBO.bind();
    dirShadowFBO.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
    currentScene->drawDirLightShadows(dirShadowShader, dirShadowFBO.depthBuffer);
    
    //As we add more error checking this will change from a dummy variable to an actual thing
    return true;
}

//TODO:: some of the buffer generation and binding should be abstracted into a function
bool RenderManager::initSSBOs(){
    //Setting up tile size on both X and Y 
    sizeX =  (unsigned int)std::ceilf(DisplayManager::SCREEN_WIDTH / (float)gridSizeX);

    float zFar    =  sceneCamera->cameraFrustum.farPlane;
    float zNear   =  sceneCamera->cameraFrustum.nearPlane;

    //Buffer containing all the clusters
    {
        glGenBuffers(1, &AABBvolumeGridSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, AABBvolumeGridSSBO);

        //We generate the buffer but don't populate it yet.
        glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(struct VolumeTileAABB), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AABBvolumeGridSSBO);
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
        //Basically reduced a log function into a simple multiplication an addition by pre-calculating these
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

    //A list of indices to the lights that are active and intersect with a cluster
    {
        unsigned int totalNumLights =  numClusters * maxLightsPerTile; //50 lights per tile max
        glGenBuffers(1, &lightIndexListSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexListSSBO);

        //We generate the buffer but don't populate it yet.
        glBufferData(GL_SHADER_STORAGE_BUFFER,  totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexListSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Every tile takes two unsigned ints one to represent the number of lights in that grid
    //Another to represent the offset to the light index list from where to begin reading light indexes from
    //This implementation is straight up from Olsson paper
    {
        glGenBuffers(1, &lightGridSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightGridSSBO);

        glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGridSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    //Setting up simplest ssbo in the world
    {
        glGenBuffers(1, &lightIndexGlobalCountSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexGlobalCountSSBO);

        //Every tile takes two unsigned ints one to represent the number of lights in that grid
        //Another to represent the offset 
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightIndexGlobalCountSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
   
    return true;
}

bool RenderManager::loadShaders(){
    bool stillValid = true;
    //Pre-processing
    stillValid &= buildAABBGridCompShader.setup("clusterShader.comp");
    stillValid &= cullLightsCompShader.setup("clusterCullLightShader.comp");
    stillValid &= fillCubeMapShader.setup("cubeMapShader.vert", "buildCubeMapShader.frag");
    stillValid &= convolveCubeMap.setup("cubeMapShader.vert", "convolveCubemapShader.frag");
    stillValid &= preFilterSpecShader.setup("cubeMapShader.vert", "preFilteringShader.frag");
    stillValid &= integrateBRDFShader.setup("screenShader.vert", "brdfIntegralShader.frag");

    if(!stillValid){
        printf("Error loading pre-processing Shaders!\n");
        return false;
    }
    //Rendering
    stillValid &= depthPrePassShader.setup("depthPassShader.vert", "depthPassShader.frag");
    stillValid &= PBRClusteredShader.setup("PBRClusteredShader.vert", "PBRClusteredShader.frag");
    stillValid &= skyboxShader.setup("skyboxShader.vert", "skyboxShader.frag");
    stillValid &= screenSpaceShader.setup("screenShader.vert", "screenShader.frag");

    if(!stillValid){
        printf("Error loading rendering Shaders!\n");
        return false;
    }

    //Shadow mapping
    stillValid &= dirShadowShader.setup("shadowShader.vert", "shadowShader.frag");
    stillValid &= pointShadowShader.setup("pointShadowShader.vert", "pointShadowShader.frag", "pointShadowShader.geom");

    if(!stillValid){
        printf("Error loading shadow mapping Shaders!\n");
        return false;
    }
    //Post-processing
    stillValid &= highPassFilterShader.setup("splitHighShader.vert", "splitHighShader.frag");
    stillValid &= gaussianBlurShader.setup("blurShader.vert", "blurShader.frag");

    if(!stillValid){
        printf("Error loading post-processing Shaders!\n");
        return false;
    }

    return stillValid;
}

void RenderManager::shutDown(){
    screen = nullptr;
    sceneCamera  = nullptr;
    sceneLocator = nullptr;
}

bool RenderManager::initFBOs(){
    //Init variables
    unsigned int shadowMapResolution = currentScene->getShadowRes();
    int skyboxRes = currentScene->mainSkyBox.resolution;
    numLights = currentScene->pointLightCount;
    bool stillValid = true;

    //Shadow Framebuffers
    pointLightShadowFBOs = new PointShadowBuffer[numLights];

    //Directional light
    stillValid  &= dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution);

    //Point light
    for(unsigned int i = 0; i < numLights; ++i ){
        stillValid &= pointLightShadowFBOs[i].setupFrameBuffer(shadowMapResolution, shadowMapResolution);
    }

    if(!stillValid){
        printf("Error initializing shadow map FBO's!\n");
        return false;
    }

    //Rendering buffers
    stillValid &= multiSampledFBO.setupFrameBuffer();
    stillValid &= captureFBO.setupFrameBuffer(skyboxRes, skyboxRes);

    if(!stillValid){
        printf("Error initializing rendering FBO's!\n");
        return false;
    }

    //Post processing buffers
    stillValid &= pingPongFBO.setupFrameBuffer();
    stillValid &= simpleFBO.setupFrameBuffer();

    if(!stillValid){
        printf("Error initializing postPRocessing FBO's!\n");
        return false;
    }

    return stillValid;
}

/* This time using volume tiled forward
Algorithm steps:
//Initialization or view frustrum change
0. Determine AABB's for each volume 
//Update Every frame
1. Depth-pre pass :: DONE
2. Mark Active tiles :: POSTPONED AS OPTIMIZATION
3. Build Tile list ::  POSTPONED AS OPTIMIZATION
4. Assign lights to tiles :: DONE (BUT SHOULD BE OPTIMIZED) 
5. Shading by reading from the active tiles list :: DONE 
6. Post processing and screen space effects :: DONE
*/
void RenderManager::render(const unsigned int start){
    //Initiating rendering gui
    ImGui::Begin("Rendering Controls");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if(ImGui::CollapsingHeader("Controls")){
        ImGui::Text("Strafe: w a s d");
        ImGui::Text("Rotate Camera: hold left click + mouse");
        ImGui::Text("Up&Down: q e");
        ImGui::Text("Reset Camera: r");
        ImGui::Text("Exit: ESC");
        ImGui::InputFloat3("Camera Pos", (float*)&sceneCamera->position); //Camera controls
        ImGui::SliderFloat("Movement speed", &sceneCamera->camSpeed, 0.005f, 1.0f);
    }
    //Making sure depth testing is enabled 
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    // Directional shadows
    dirShadowFBO.bind();
    dirShadowFBO.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
    currentScene->drawDirLightShadows(dirShadowShader, dirShadowFBO.depthBuffer);

    //1.1- Multisampled Depth pre-pass
    multiSampledFBO.bind();
    multiSampledFBO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::vec3(0.0f));
    currentScene->drawDepthPass(depthPrePassShader);

    //4-Light assignment
    cullLightsCompShader.use();
    cullLightsCompShader.setMat4("viewMatrix", sceneCamera->viewMatrix);
    cullLightsCompShader.dispatch(1,1,6);  

    //5 - Actual shading;
    //5.1 - Forward render the scene in the multisampled FBO using the z buffer to discard early
    glDepthFunc(GL_LEQUAL);
    glDepthMask(false);
    currentScene->drawFullScene(PBRClusteredShader, skyboxShader);

    //5.2 - resolve the from multisampled to normal resolution for postProcessing
    multiSampledFBO.blitTo(simpleFBO, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //6 -postprocessing, includes bloom, exposure mapping
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

    //TODO:: should be a compute shader 
    pingPongFBO.bind();
    pingPongFBO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::vec3(0.0f));
    if( sceneCamera->blurAmount > 0){
        //Filtering pixel rgb values > 1.0
        highPassFilterShader.use();
        canvas.draw(simpleFBO.texColorBuffer);
    }

    //Applying Gaussian blur in ping pong fashion
    //TODO:: ALso make it a compute shader
    gaussianBlurShader.use();
    for (int i = 0; i < sceneCamera->blurAmount; ++i){
        //Horizontal pass
        glBindFramebuffer(GL_FRAMEBUFFER, simpleFBO.frameBufferID);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        gaussianBlurShader.setBool("horizontal", true);
        canvas.draw(pingPongFBO.texColorBuffer);

        //Vertical pass
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO.frameBufferID);
        gaussianBlurShader.setBool("horizontal", false);
        canvas.draw(simpleFBO.blurHighEnd);
    }
    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    screen->bind();

    //Shader setup for postprocessing
    screenSpaceShader.use();

    screenSpaceShader.setInt("offset", start);
    screenSpaceShader.setFloat("exposure", sceneCamera->exposure);
    screenSpaceShader.setInt("screenTexture", 0);
    screenSpaceShader.setInt("bloomBlur", 1);
    screenSpaceShader.setInt("computeTexture", 2);

    //Merging the blurred high pass image with the low pass values
    //Also tonemapping and doing other post processing
    canvas.draw(simpleFBO.texColorBuffer, pingPongFBO.texColorBuffer);
}
