#ifndef SCENE_H
#define SCENE_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-12
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Headers
#include <vector>
#include "model.h"
#include "camera.h"
#include "texture.h"
#include "skybox.h"
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"
#include "light.h"

using json = nlohmann::json;

class Scene{
    public:
        //Builds scene using path to folder containing content and txt setup file
        Scene(const std::string &sceneFolder);
        ~Scene();

        //Updates all models, lights and cameras
        void update(unsigned int deltaT);

        //Deffered lighting drawing functions
        void drawGeometry(Shader *gBufferShader);
        void setupLightingShader(Shader *lightShader);

        //Old Forward renderer drawing functions
        void drawPointLightShadow(Shader *pointLightShader, unsigned int index, unsigned int cubeMapTarget);
        void drawDirLightShadows(Shader *dirLightShader, unsigned int targetTextureID);
        void drawFullScene(Shader *mainSceneShader, Shader *skyboxShader);
        void drawDepthPass(Shader *depthPassShader);

        //Getters used in the setup of the render queue
        std::vector<Model*>* getVisiblemodels();
        Camera * getCurrentCamera();
        PointLight *getPointLight(unsigned int index);
        unsigned int getShadowRes();

        //Signals issues to scene Manager
        bool checkIfEmpty();  

        //Tired of making things private, making them public as I go and we'll fix the rest later
        CubeMap irradianceMap, specFilteredMap;
        Texture brdfLUTTexture;
        Skybox mainSkyBox;
        unsigned int pointLightCount;

        Camera *mainCamera;

    private:
        const std::string folderPath = "../assets/scenes/";
        const std::string fileExtension = ".json";

        std::string sceneID;
        bool missingScene;

        DirectionalLight dirLight;

        PointLight *pointLights;

        //Contains the models that remain after frustrum culling
        std::vector<Model*> visibleModels;
        std::vector<Model*> modelsInScene;

        //Scene loading
        bool loadContent();
        void loadSceneModels(const json &sceneConfigJson);
        void loadSkyBox(const json &sceneConfigJson);
        void loadLights(const json &sceneConfigJson);
        void loadCamera(const json &sceneConfigJson);
        void generateEnvironmentMaps();
        
        //Finds objects that the camera can see
        void frustrumCulling();
};

#endif