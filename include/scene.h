#ifndef SCENE_H
#define SCENE_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-12
PURPOSE      : 
SPECIAL NOTES: 
*/

//Headers
#include <vector>
#include "model.h"
#include "camera.h"
#include "texture.h"
#include "cubeMap.h"
#include "skybox.h"
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"
#include "light.h"

using json = nlohmann::json;

class Scene{
    public:
        //Builds scene using a string to a JSON scene description file
        Scene(const std::string &sceneFolder);
        ~Scene();

        //Updates all models, lights and cameras
        void update(unsigned int deltaT);

        //Old Forward renderer drawing functions
        void drawPointLightShadow(const Shader &pointLightShader, unsigned int index, unsigned int cubeMapTarget);
        void drawDirLightShadows(const Shader &dirLightShader, unsigned int targetTextureID);
        void drawFullScene(const Shader &mainSceneShader, const Shader &skyboxShader);
        void drawDepthPass(const Shader &depthPassShader);

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
        std::string sceneID;
        bool missingScene;

        DirectionalLight dirLight;

        PointLight *pointLights;

        //Contains the models that remain after frustrum culling
        std::vector<Model*> visibleModels;
        std::vector<Model*> modelsInScene;

        //Scene loading
        bool loadContent();
        void generateEnvironmentMaps();
        void loadSkyBox(const json &sceneConfigJson);
        void loadLights(const json &sceneConfigJson);
        void loadCamera(const json &sceneConfigJson);
        void loadSceneModels(const json &sceneConfigJson);
        
        //Builds the list of meshes that are visible
        //Currently disabled working for rework of model/mesh
        void frustrumCulling();
};

#endif