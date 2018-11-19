/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-12
*/

//Headers
#include "scene.h"
#include "fileManager.h"
#include <iostream>
#include <fstream>
#include "imgui/imgui.h"

Scene::Scene(const std::string &sceneName){
    sceneID = sceneName;
    if( !FLOAD::checkFileValidity(folderPath + sceneName + fileExtension) ){
        //If you do not find the scene file set the quit flag to true 
        printf("Cannot find scene descriptor file for %s \n", sceneID.c_str());
        missingScene = true; 
    }
    else{
        //Load all cameras, models and lights and return false if it fails
        missingScene = !loadContent();
    }
}

Scene::~Scene(){
    //Making sure you don't attempt to delete models that don't exist
    if (!missingScene){
        for(Model *models : modelsInScene){
            delete models;
        }
        delete [] pointLights;
        delete  mainCamera;
    }
}

//Update Order is critical for correct culling
void Scene::update(unsigned int deltaT){
    visibleModels.clear();
    mainCamera->update(deltaT);
    // for(int i=0; i < lightCount; ++i){
    //     lights[i].update(deltaT);
    // }
    for(Model *model : modelsInScene){
        model->update(deltaT);
    }
    frustrumCulling();
}

void Scene::drawPointLightShadow(Shader *pointLightShader, unsigned int index, unsigned int cubeMapTarget){
    //Current light
    PointLight * light = &pointLights[index];
    light->depthMapTextureID = cubeMapTarget;
    //Shader setup
    pointLightShader->use();
    pointLightShader->setVec3("lightPos", light->position);
    pointLightShader->setFloat("far_plane", light->zFar);

    //Matrix setup
    glm::mat4 lightMatrix, M;
    glm::mat4 shadowProj = light->shadowProjectionMat;
    for (unsigned int face = 0; face < PointLight::numFaces; ++face){
        std::string number = std::to_string(face);
        lightMatrix = shadowProj * light->lookAtPerFace[face];
        pointLightShader->setMat4(("shadowMatrices[" + number + "]").c_str(), lightMatrix);
    }

    for(unsigned int i = 0; i < modelsInScene.size(); ++i){
        Model * currentModel = modelsInScene[i];

        M = currentModel->getModelMatrix();
        //Shader setup stuff that changes every frame
        pointLightShader->setMat4("M", M);
        
        //Draw object
        currentModel->draw(*pointLightShader, false);
    }
}

void Scene::drawDirLightShadows(Shader *dirLightShader, unsigned int targetTextureID){
    glm::mat4 ModelLS = glm::mat4(1.0);
    dirLight.depthMapTextureID = targetTextureID;

    float left = dirLight.orthoBoxSize;
    float right = -left;
    float top = left;
    float bottom = -top;
    dirLight.shadowProjectionMat = glm::ortho(left, right, bottom, top, dirLight.zNear, dirLight.zFar);
    dirLight.lightView = glm::lookAt(dirLight.distance * -dirLight.direction,
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));

    dirLight.lightSpaceMatrix = dirLight.shadowProjectionMat * dirLight.lightView;

    //Drawing every object into the shadow buffer
    for(unsigned int i = 0; i < modelsInScene.size(); ++i){
        Model * currentModel = modelsInScene[i];

        //Matrix setup
        ModelLS = dirLight.lightSpaceMatrix * currentModel->getModelMatrix();

        //Shader setup stuff that changes every frame
        dirLightShader->use();
        dirLightShader->setMat4("lightSpaceMatrix", ModelLS);
        
        //Draw object
        currentModel->draw(*dirLightShader, false);
    }
}

void Scene::drawGeometry(Shader *gBufferShader){
    //Matrix Setup
    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 M   = glm::mat4(1.0);
    glm::mat4 VP  = mainCamera->projectionMatrix * mainCamera->viewMatrix;

    gBufferShader->use();

    for(unsigned int i = 0; i < visibleModels.size(); ++i){
        Model * currentModel = visibleModels[i];

        //Matrix setup
        M  = currentModel->getModelMatrix();
        MVP = VP * M;

        //Shader setup stuff that changes every frame
        gBufferShader->setMat4("MVP", MVP);
        gBufferShader->setMat4("M", M);
        
        //Draw object
        currentModel->draw(*gBufferShader, true);
    }
}

void Scene::setupLightingShader(Shader *lightShader){
    const unsigned int numTextures =  3;
    lightShader->use();
    //Setting up gBuffer texture uniform positions 
    lightShader->setInt("position", 0);
    lightShader->setInt("normals", 1);
    lightShader->setInt("albedoSpec", 2);

    if(ImGui::CollapsingHeader("Directional Light Settings")){
        ImGui::TextColored(ImVec4(1,1,1,1), "Directional light Settings");
        ImGui::ColorEdit3("Color", (float *)&dirLight.color);
        ImGui::SliderFloat("Strength", &dirLight.strength, 0.1f, 200.0f);
        ImGui::SliderFloat("BoxSize", &dirLight.orthoBoxSize, 0.1f, 500.0f);
        ImGui::SliderFloat("Distance", &dirLight.distance, 0.1f, 500.0f);
        ImGui::SliderFloat3("Direction", (float*)&dirLight.direction, -5.0f, 5.0f);
        ImGui::InputFloat3("Camera Position", (float*)&mainCamera->position);
    }

    //Setting up directional light uniforms
    lightShader->setVec3("dirLight.direction", dirLight.direction);
    lightShader->setVec3("dirLight.ambient",   dirLight.ambient);
    lightShader->setVec3("dirLight.diffuse",   dirLight.strength * dirLight.color * dirLight.diffuse);
    lightShader->setVec3("dirLight.specular",  dirLight.specular);
    lightShader->setMat4("dirLight.lightSpaceMatrix", dirLight.lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE0 + numTextures);
    lightShader->setInt("shadowMap", numTextures);
    glBindTexture(GL_TEXTURE_2D, dirLight.depthMapTextureID);

    //Point lights
    for (unsigned int i = 0; i < pointLightCount; ++i)
    {
        PointLight *light = &pointLights[i];
        std::string number = std::to_string(i);

        // //Point light Gui
        // // static ImVec4 color = ImColor(light->color.r, light->color.g, light->color.b);
        // ImGui::TextColored(ImVec4(1, 1, 1, 1), ("Light" + number).c_str());
        // ImGui::SliderFloat(("Strength" + number).c_str(), &(light->strength), 0.1f, 200.0f);
        // ImGui::ColorEdit3(("Color" + number).c_str(), (float *)&(light->color));
        // ImGui::InputFloat3(("Position" + number).c_str(), (float *)&(light->position));

        lightShader->setVec3(("pointLights[" + number + "].position").c_str(), light->position);
        lightShader->setVec3(("pointLights[" + number + "].ambient").c_str(), light->ambient);
        lightShader->setVec3(("pointLights[" + number + "].diffuse").c_str(), light->strength * light->color * light->diffuse);
        lightShader->setVec3(("pointLights[" + number + "].specular").c_str(), light->specular);
        lightShader->setFloat(("pointLights[" + number + "].constant").c_str(), light->attenuation[0]);
        lightShader->setFloat(("pointLights[" + number + "].linear").c_str(), light->attenuation[1]);
        lightShader->setFloat(("pointLights[" + number + "].quadratic").c_str(), light->attenuation[2]);

        //Change constants herer TODO TODO
        glActiveTexture(GL_TEXTURE0 + numTextures + 1 + i);
        lightShader->setInt(("pointLights[" + number + "].depthMap").c_str(), numTextures + 1 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthMapTextureID);
        lightShader->setFloat("far_plane", light->zFar);
    }
    //Camera Uniforms
    lightShader->setVec3("cameraPos_wS", mainCamera->position);
}

void Scene::drawFullScene(Shader *mainSceneShader, Shader *skyboxShader){
    //Matrix Setup
    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 M   = glm::mat4(1.0);
    glm::mat4 VP  = mainCamera->projectionMatrix * mainCamera->viewMatrix;
    glm::mat4 VPCubeMap = mainCamera->projectionMatrix *glm::mat4(glm::mat3(mainCamera->viewMatrix));

    //Just to avoid magic constants
    const unsigned int numTextures =  5;

    //Setting colors in the gui
    if(ImGui::CollapsingHeader("Directional Light Settings")){
        ImGui::TextColored(ImVec4(1,1,1,1), "Directional light Settings");
        ImGui::ColorEdit3("Color", (float *)&dirLight.color);
        ImGui::SliderFloat("Strength", &dirLight.strength, 0.1f, 200.0f);
        ImGui::SliderFloat("BoxSize", &dirLight.orthoBoxSize, 0.1f, 500.0f);
        ImGui::SliderFloat("Distance", &dirLight.distance, 0.1f, 500.0f);
        ImGui::SliderFloat3("Direction", (float*)&dirLight.direction, -5.0f, 5.0f);
    }

    mainSceneShader->use();
    mainSceneShader->setVec3("dirLight.direction", dirLight.direction);
    mainSceneShader->setVec3("dirLight.color",   dirLight.strength * dirLight.color);
    mainSceneShader->setMat4("lightSpaceMatrix", dirLight.lightSpaceMatrix);
    mainSceneShader->setVec3("cameraPos_wS", mainCamera->position);
    mainSceneShader->setFloat("zFar", mainCamera->cameraFrustum.farPlane);
    mainSceneShader->setFloat("zNear", mainCamera->cameraFrustum.nearPlane);

    if(ImGui::CollapsingHeader("PointLights", ImGuiTreeNodeFlags_DefaultOpen)){
        for (unsigned int i = 0; i < pointLightCount; ++i)
        {
            PointLight *light = &pointLights[i];
            std::string number = std::to_string(i);

            //Point light Gui
            // static ImVec4 color = ImColor(light->color.r, light->color.g, light->color.b);
            // ImGui::TextColored(ImVec4(1, 1, 1, 1), ("Light" + number).c_str());
            // ImGui::SliderFloat(("Strength" + number).c_str(), &(light->strength), 0.1f, 200.0f);
            // ImGui::ColorEdit3(("Color" + number).c_str(), (float *)&(light->color));
            // ImGui::InputFloat3(("Position"+ number).c_str(), (float*)&(light->position));

            // mainSceneShader->setVec3(("pointLights[" + number + "].position").c_str(), light->position); 
            // mainSceneShader->setVec3(("pointLight_wS[" + number + "]").c_str(), light->position); 
            // mainSceneShader->setVec3(("pointLights[" + number + "].ambient").c_str(), light->ambient);
            // mainSceneShader->setVec3(("pointLights[" + number + "].diffuse").c_str(), light->strength * light->color * light->diffuse);
            // mainSceneShader->setVec3(("pointLights[" + number + "].specular").c_str(), light->specular);
            // mainSceneShader->setFloat(("pointLights[" + number + "].constant").c_str(), light->attenuation[0]);
            // mainSceneShader->setFloat(("pointLights[" + number + "].linear").c_str(), light->attenuation[1]);
            // mainSceneShader->setFloat(("pointLights[" + number + "].quadratic").c_str(), light->attenuation[2]);

            glActiveTexture(GL_TEXTURE0 + numTextures + i); 
            mainSceneShader->setInt(("depthMaps[" + number + "]").c_str(), numTextures + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthMapTextureID);
            mainSceneShader->setFloat("far_plane", light->zFar);
        }   
    }    
    //Setting directional shadow depth map textures
    glActiveTexture(GL_TEXTURE0 + numTextures + pointLightCount);
    mainSceneShader->setInt("shadowMap", numTextures + pointLightCount);
    glBindTexture(GL_TEXTURE_2D, dirLight.depthMapTextureID);


    //TODO:: Formalize htis a bit more
    //Setting environment map texture
    glActiveTexture(GL_TEXTURE0 + numTextures + pointLightCount + 1);
    mainSceneShader->setInt("irradianceMap", numTextures + pointLightCount + 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap.textureID);

    //Setting environment map texture for specular
    glActiveTexture(GL_TEXTURE0 + numTextures + pointLightCount + 2);
    mainSceneShader->setInt("prefilterMap", numTextures + pointLightCount + 2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specFilteredMap.textureID);

    //Setting lookup table
    glActiveTexture(GL_TEXTURE0 + numTextures + pointLightCount + 3);
    mainSceneShader->setInt("brdfLUT", numTextures + pointLightCount + 3);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture.textureID);

    for(unsigned int i = 0; i < visibleModels.size(); ++i){
        Model * currentModel = visibleModels[i];

        //Matrix setup
        M  = currentModel->getModelMatrix();
        MVP = VP * M;

        //Shader setup stuff that changes every frame
        mainSceneShader->setMat4("MVP", MVP);
        mainSceneShader->setMat4("M", M);
        
        //Draw object
        currentModel->draw(*mainSceneShader, true);
    }

    //Drawing skybox
    skyboxShader->use();
    skyboxShader->setMat4("VP", VPCubeMap);
    mainSkyBox.draw();
}

void Scene::drawDepthPass(Shader *depthPassShader){
    //Matrix Setup
    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 VP  = mainCamera->projectionMatrix * mainCamera->viewMatrix;

    glColorMask(0,0,0,0);
    //Drawing every object into the depth buffer
    for(unsigned int i = 0; i < modelsInScene.size(); ++i){
        Model * currentModel = modelsInScene[i];

        //Matrix setup
        MVP = VP * currentModel->getModelMatrix();

        //Shader setup stuff that changes every frame
        depthPassShader->use();
        depthPassShader->setMat4("MVP", MVP);
        
        //Draw object
        currentModel->draw(*depthPassShader, false);
    }
    glColorMask(1,1,1,1);
}

//-----------------------------GETTERS----------------------------------------------
std::vector<Model*>* Scene::getVisiblemodels(){
    return &visibleModels;
}

Camera* Scene::getCurrentCamera(){
    return mainCamera;
}

unsigned int Scene::getShadowRes(){
    return dirLight.shadowRes;
}

PointLight *Scene::getPointLight(unsigned int index){
    return &pointLights[index];
}
//----------------------------------------------------------------
bool Scene::checkIfEmpty(){
    return missingScene;
}
//-----------------------------SCENE LOADING-----------------------------------

//Config file parsing, gets all the important 
bool Scene::loadContent(){
    //Parsing into Json file readable format
    std::string sceneConfigFilePath = folderPath + sceneID + fileExtension;
    std::ifstream file(sceneConfigFilePath.c_str());
    json configJson;
    file >> configJson;

    //Checking that config file belongs to current scene and is properly formatted
    if (configJson["sceneID"] != sceneID & ((unsigned int)configJson["models"].size() != 0)){
        printf("Error! Config file: %s does not belong to current scene, check configuration.\n", sceneConfigFilePath.c_str());
        return false;
    }
    //now we can parse the rest of the file "safely"
    //TODO: more safety checks
    else{
        printf("Loading camera...\n");
        loadCamera(configJson);

        printf("Loading models...\n");
        loadSceneModels(configJson);

        printf("Loading skybox...\n");
        loadSkyBox(configJson);

        printf("Loading lights...\n");
        loadLights(configJson);

        printf("Generating environment maps...\n");
        generateEnvironmentMaps();

        printf("Reticulating splines...\n");

        //lastly we check if the scene is empty and return
        printf("Loading Complete!...\n");
        return !modelsInScene.empty();
    }
}

void Scene::loadLights(const json &sceneConfigJson){
    //Directional light
    printf("Loading directional light...\n");
    {
        json light = sceneConfigJson["directionalLight"];

        //Vector values
        dirLight.ambient  = glm::vec3((float)light["ambient"]);
        dirLight.diffuse  = glm::vec3((float)light["diffuse"]);
        dirLight.specular = glm::vec3((float)light["specular"]);

        json direction = light["direction"];
        dirLight.direction = glm::vec3((float)direction[0],
                                        (float)direction[1],
                                        (float)direction[2]);

        json color = light["color"];
        dirLight.color = glm::vec3((float)color[0],
                                        (float)color[1],
                                        (float)color[2]);
                                        
        //Scalar values
        dirLight.distance = (float)light["distance"];
        dirLight.strength = (float)light["strength"];
        dirLight.zNear = (float)light["zNear"];
        dirLight.zFar = (float)light["zFar"];
        dirLight.orthoBoxSize = (float)light["orthoSize"];
        dirLight.shadowRes = (unsigned int)light["shadowRes"];

        //Matrix values
        float left   = dirLight.orthoBoxSize;
        float right  = -left;
        float top    = left;
        float bottom = -top;
        dirLight.shadowProjectionMat = glm::ortho(left, right, bottom, top, dirLight.zNear, dirLight.zFar);
        dirLight.lightView = glm::lookAt(dirLight.distance * -dirLight.direction,
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f));

        dirLight.lightSpaceMatrix = dirLight.shadowProjectionMat * dirLight.lightView;
    }
    //Point lights
    printf("Loading point light...\n");
    {
        //Get number of lights in scene and initialize array containing them
        pointLightCount = (unsigned int)sceneConfigJson["pointLights"].size();
        pointLights = new PointLight[pointLightCount];

        for(unsigned int i = 0; i < pointLightCount; ++i){
            json light = sceneConfigJson["pointLights"][i];
            //Vector values
            pointLights[i].ambient = glm::vec3((float)light["ambient"]);
            pointLights[i].diffuse = glm::vec3((float)light["diffuse"]);
            pointLights[i].specular = glm::vec3((float)light["specular"]);

            json position = light["position"];
            pointLights[i].position = glm::vec3((float)position[0],
                                           (float)position[1],
                                           (float)position[2]);

            json color = light["color"];
            pointLights[i].color = glm::vec3((float)color[0],
                                       (float)color[1],
                                       (float)color[2]);

            json attenuation = light["attenuation"];
            pointLights[i].attenuation = glm::vec3((float)attenuation[0],
                                       (float)attenuation[1],
                                       (float)attenuation[2]);

            //Scalar values
            pointLights[i].strength = (float)light["strength"];
            pointLights[i].zNear = (float)light["zNear"];
            pointLights[i].zFar = (float)light["zFar"];
            pointLights[i].shadowRes = (unsigned int)light["shadowRes"];
            //Always using cubefaces so we can hard code this to 1
            pointLights[i].aspect = 1.0f;

            //Matrix setup
            pointLights[i].shadowProjectionMat = glm::perspective(pointLights[i].ang,
                pointLights[i].aspect, pointLights[i].zNear,pointLights[i].zFar);
            
            glm::vec3 lightPos = pointLights[i].position;
            pointLights[i].lookAtPerFace[0] = glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
            pointLights[i].lookAtPerFace[1] = glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
            pointLights[i].lookAtPerFace[2] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
            pointLights[i].lookAtPerFace[3] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
            pointLights[i].lookAtPerFace[4] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
            pointLights[i].lookAtPerFace[5] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
        }
    }
}

void Scene::loadSkyBox(const json &sceneConfigJson){
    json skyBox = sceneConfigJson["skybox"];
    std::string skyBoxName = skyBox["id"];
    bool isHDR = skyBox["hdr"];
    int resolution = skyBox["resolution"];

    mainSkyBox.setup(skyBoxName, isHDR, resolution);
}

void Scene::loadSceneModels(const json &sceneConfigJson ){
    //model setup
    std::string modelMesh, modelName;
    bool pbrEnabled;
    TransformParameters initParameters;
    unsigned int modelCount = (unsigned int)sceneConfigJson["models"].size();


    for (unsigned int i = 0; i < modelCount; ++i){
        //get model mesh and material info
        json currentModel = sceneConfigJson["models"][i];
        modelMesh = currentModel["mesh"];
        pbrEnabled = currentModel["pbr"];

        modelName = modelMesh.substr(0, modelMesh.find_last_of('.'));

        //position
        json position = currentModel["position"];
        initParameters.translation = glm::vec3((float)position[0], (float)position[1], (float)position[2]);

        //rotation
        json rotation = currentModel["rotation"];
        initParameters.angle = glm::radians((float)rotation[0]);
        initParameters.rotationAxis = glm::vec3((float)rotation[1],
                                            (float)rotation[2],
                                            (float)rotation[3]);
        // initParameters.rotationAxis = glm::vec3(glm::radians((float)rotation[1]),
        //                                     glm::radians((float)rotation[2]),
        //                                     glm::radians((float)rotation[3]));

        //scaling
        json scaling = currentModel["scaling"];
        initParameters.scaling = glm::vec3((float)scaling[0], (float)scaling[1], (float)scaling[2]);

        //attempts to load model with the initparameters it has read
        modelMesh = "../assets/models/" + modelName + "/" + modelMesh;
        if (!FLOAD::checkFileValidity(modelMesh)){
            //If the mesh deos not exist it's very likely nothing else does, quit early
            printf("Error! Mesh: %s does not exist.\n", modelMesh.c_str());
        }
        else{
            modelsInScene.push_back(new Model(modelMesh, pbrEnabled, initParameters));
        }
    }
}
//TODO move the fixed size somewhere else
void Scene::generateEnvironmentMaps(){
    //Diffuse map
    irradianceMap.width = 32;
    irradianceMap.height = 32;
    irradianceMap.generateCubeMap(irradianceMap.width, irradianceMap.height, HDR_MAP);

    //Specular map
    specFilteredMap.width = 128;
    specFilteredMap.height = 128;
    specFilteredMap.generateCubeMap(specFilteredMap.width, specFilteredMap.height, PREFILTER_MAP);

    //Setting up texture ahead of time
    unsigned int res = 512;
    brdfLUTTexture.height = res;
    brdfLUTTexture.width  = res;
    glGenTextures(1, &brdfLUTTexture.textureID);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture.textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, res, res, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Scene::loadCamera(const json &sceneConfigJson){
    json cameraSettings = sceneConfigJson["camera"];
    float speed = (float)cameraSettings["speed"];
    float sens  = (float)cameraSettings["mouseSens"];
    float fov   = (float)cameraSettings["fov"];
    float nearP = (float)cameraSettings["nearPlane"];
    float farP  = (float)cameraSettings["farPlane"];

    json position = cameraSettings["position"];
    glm::vec3 pos = glm::vec3((float)position[0], (float)position[1], (float)position[2]);

    json target   = cameraSettings["target"];
    glm::vec3 tar = glm::vec3((float)target[0], (float)target[1], (float)target[2]);

    mainCamera = new Camera(tar, pos, fov, speed, sens, nearP, farP);
}
//-------------------------------------------------------------
//TODO TODO TODO TODO TODO TODO TODO
void Scene::frustrumCulling(){
    for(Model *model : modelsInScene){
        visibleModels.push_back(model);
        // bool visible = mainCamera.checkVisibility(model->getBounds());
        // if (visible) {
        // }
    }
}