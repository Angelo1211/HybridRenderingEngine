// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-13
// ===============================

//Includes
#include "renderManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

//Temporarily here
GLenum RenderManager::glCheckError_(const char *file, int line){
    GLenum errorCode;
    while((errorCode = glGetError()) != GL_NO_ERROR){
        std::string error;
        switch(errorCode){
            case GL_INVALID_ENUM: error = "Invalid Enum"; break;
            case GL_INVALID_VALUE: error = "Invalid value"; break;
            case GL_INVALID_OPERATION: error = "Invalid operation"; break;
            case GL_STACK_OVERFLOW: error = "stack overflow"; break;
            case GL_STACK_UNDERFLOW: error = "stack underflow"; break;
            case GL_OUT_OF_MEMORY: error = "out of memory"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "invalid framebuffer operation"; break;
        }
        printf("%s, | %s ( %d ) \n", error.c_str(), file, line);
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

//Dummy constructors / Destructors
RenderManager::RenderManager(){}
RenderManager::~RenderManager(){}

//Sets the internal pointers to the screen and the current scene and inits the software
//renderer instance. 
bool RenderManager::startUp(DisplayManager &displayManager, SceneManager &sceneManager ){
    screen = &displayManager;
    sceneLocator = &sceneManager;

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
            if( !setupQuad()){
                printf("Final drawing quad failed to instantiate! \n");
                return false; }
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


    sceneCamera  = nullptr;
    sceneLocator = nullptr;
    screen = nullptr;
}

bool RenderManager::initFBOs(){
    bool initFBOFlag1 = multiSampledFBO.setupFrameBuffer(true);
    bool initFBOFlag2 = simpleFBO.setupFrameBuffer();
    bool initFBOFLag3 = dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution, false);
    
    bool initFBOFlagPointLights = true;
    for(unsigned int i = 0; i < 4; ++i ){
        initFBOFlagPointLights = initFBOFlagPointLights && pointLightShadowFBOs[i].setupFrameBuffer(shadowMapResolution, shadowMapResolution, true);
    }

    return initFBOFlag1 && initFBOFlag2 && initFBOFLag3 && initFBOFlagPointLights;
}

void RenderManager::render(const unsigned int start){
    //Imgui frame setup
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(screen->getWindow());
    ImGui::NewFrame();

    ImGui::Begin("Rendering Controls");

    glEnable(GL_DEPTH_TEST);
    
    //Preps all the items that will be drawn in the scene
    buildRenderQueue();

    if(hasMoved){
        hasMoved = false;

        //Populating depth cube maps for the point lights
        for (unsigned int i = 0; i < 4; ++i){
            pointLightShadowFBOs[i].bind();
            drawPointShadow(i);
        }

        // Directional shadows
        dirShadowFBO.bind();
        drawDirShadow();
    }
    // glCheckError();
    //Set the multisampled FBO as the first render target
    multiSampledFBO.bind();

    //First we draw the scene as normal but on the offscreen buffer
    drawScene();

    // glCheckError();
    //Resolving the multisampled buffer into a regular one for postprocessing
    multiSampledFBO.blitTo(simpleFBO);

    //Setting back to default framebuffer (screen) and clearing
    //No need for depth testing cause we're drawing to a flat quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Render to quad and apply postprocessing effects
    postProcess(start);


    //Closing Rendering Window and rendering all gui stuff
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    // glCheckError();
    //Drawing to the screen by swapping the window's surface with the
    //final buffer containing all rendering information
    screen->swapDisplayBuffer();

    // glCheckError();
    //Set camera pointer to null just in case a scene change occurs
    sceneCamera = nullptr;
}

void RenderManager::drawPointShadow(int i){

            //Setup matrices and shader
            float aspect = (float)pointLightShadowFBOs[i].width / (float)pointLightShadowFBOs[i].height;
            float zNear = 1.0f;
            float zFar = 2000.0f;
            float const ang = glm::radians(90.0f);
            glm::mat4 shadowProj = glm::perspective(ang, aspect, zNear, zFar);

            //look at matrix setup
            glm::mat4 lookAtPerFace[6];
            glm::vec3 lightPos = pointLightPositions[i];
            lookAtPerFace[0] = glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
            lookAtPerFace[1] = glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
            lookAtPerFace[2] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
            lookAtPerFace[3] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
            lookAtPerFace[4] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
            lookAtPerFace[5] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

            //Shader setup
            shaderAtlas[4]->use();
            shaderAtlas[4]->setVec3("lightPos", lightPos);
            shaderAtlas[4]->setFloat("far_plane", zFar);

            for (unsigned int i = 0; i < 6; ++i)
            {
                std::string number = std::to_string(i);
                lookAtPerFace[i] = shadowProj * lookAtPerFace[i];
                shaderAtlas[4]->setMat4(("shadowMatrices[" + number + "]").c_str(), lookAtPerFace[i]);
            }

            glm::mat4 M = glm::mat4(1.0);
            for (unsigned int i = 0; i < renderObjectQueue->size(); ++i)
            {
                Model *currentModel = (*renderObjectQueue)[i];

                M = currentModel->getModelMatrix();

                //Shader setup stuff that changes every frame
                shaderAtlas[4]->setMat4("M", M);

                //Draw object
                currentModel->draw(*shaderAtlas[4], false);
            }
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
    shaderAtlas[4] = new Shader("pointShadowShader.vert", "pointShadowShader.frag", "pointShadowShader.geom");


    // shaderAtlas[2]->use();
    // shaderAtlas[2]->setInt("skybox", 0);

    return ( shaderAtlas[0] != nullptr ) && ( shaderAtlas[1] != nullptr ) && ( shaderAtlas[2] != nullptr );
}

void RenderManager::drawDirShadow(){
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
        currentModel->draw(*shaderAtlas[3], false);
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

    ImGui::SliderFloat("Sun Strength", &dirLightStrength, 0.1f, 200.0f);
    shaderAtlas[0]->setVec3("dirLight.direction", -dirLightPosition);
    shaderAtlas[0]->setVec3("dirLight.ambient",   glm::vec3(0.02f));
    shaderAtlas[0]->setVec3("dirLight.diffuse",   glm::vec3(dirLightStrength));
    shaderAtlas[0]->setVec3("dirLight.specular",  glm::vec3(0.5f));

    for(unsigned int i = 0; i < 4; ++i){
        std::string number = std::to_string(i);

        shaderAtlas[0]->setVec3(("pointLights["  + number + "].position").c_str(), pointLightPositions[i]);
        shaderAtlas[0]->setVec3(("pointLights["  + number + "].ambient").c_str(), glm::vec3(0.05f));
        shaderAtlas[0]->setVec3(("pointLights["  + number + "].diffuse").c_str(), pointLightColor[i]);
        shaderAtlas[0]->setVec3(("pointLights["  + number + "].specular").c_str(), glm::vec3(0.4f));
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].constant").c_str(), 1.0f);
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].linear").c_str(), 0.007f);
        shaderAtlas[0]->setFloat(("pointLights[" + number + "].quadratic").c_str(), 0.0002f);

        glActiveTexture(GL_TEXTURE2 + i);
        shaderAtlas[0]->setInt(("pointLights[" + number + "].depthMap").c_str(), 2 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightShadowFBOs[i].depthMap);
        shaderAtlas[0]->setFloat("far_plane", 2000.0f);
    }

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

        glActiveTexture(GL_TEXTURE6);
        shaderAtlas[0]->setInt("shadowMap", 6);
        glBindTexture(GL_TEXTURE_2D, dirShadowFBO.depthMap);
        
        //Draw object
        currentModel->draw(*shaderAtlas[0], true);
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
    ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
    //Shader setup for postprocessing
    shaderAtlas[1]->use();
    shaderAtlas[1]->setInt("offset", start);
    shaderAtlas[1]->setFloat("exposure", exposure);

    //Switching to the VAO of the quad and binding the texture buffer with
    // frame drawn off-screen
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    // glBindTexture(GL_TEXTURE_2D, shadowFBO.depthMap);
    glBindTexture(GL_TEXTURE_2D, simpleFBO.texColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

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
