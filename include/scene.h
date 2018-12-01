#ifndef SCENE_H
#define SCENE_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-12
PURPOSE      : Store all the data that is necessary for rendering, such as meshes, textures
               cameras, lights, skyboxes and environment maps or Look up textures.
SPECIAL NOTES: Should make json file parsing it's own namespace and call that instead.
               Also start thinking about serialization
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

        //Forward rendering drawing functions. 
        //The reason these are called here and not inside the renderer class is that most of the data 
        //is located within this class. There might be an opportunity to restructure this in a more data oriented 
        //way, but I don't see how just yet. Possibly just keeping a struct of arrays of all the items in the scene?
        void drawPointLightShadow(const Shader &pointLightShader, unsigned int index, unsigned int cubeMapTarget);
        void drawDirLightShadows(const Shader &dirLightShader, unsigned int targetTextureID);
        void drawFullScene(const Shader &mainSceneShader, const Shader &skyboxShader);
        void drawDepthPass(const Shader &depthPassShader);

        //Getters used in the setup of the render queue
        std::vector<Model*>* getVisiblemodels();
        Camera * getCurrentCamera();
        PointLight *getPointLight(unsigned int index);
        unsigned int getShadowRes();

        //Tired of making things private, making them public as I go and we'll fix the rest later
        CubeMap irradianceMap, specFilteredMap;
        Texture brdfLUTTexture;
        Skybox mainSkyBox;
        unsigned int pointLightCount;
        bool loadingError;

        //TODO:: No real reason this should be a pointer, it could be treated like the other objects
        //which just initialize their data separately from their constructor
        Camera *mainCamera;

    private:
        std::string sceneID;
        bool slices = false;

        //TODO:: unify light model so that we can have a single array pointing to a base class (or single class)
        //so that we can iterate through it
        DirectionalLight dirLight;
        PointLight *pointLights;

        //Contains the models that remain after frustrum culling which is TB done
        std::vector<Model*> visibleModels;
        std::vector<Model*> modelsInScene;

        //Scene loading functions and environment map generation (but not actually filling the memory just yet)
        bool loadContent();
        void generateEnvironmentMaps();
        void loadSkyBox(const json &sceneConfigJson);
        void loadLights(const json &sceneConfigJson);
        void loadCamera(const json &sceneConfigJson);
        void loadSceneModels(const json &sceneConfigJson);
        
        //Builds the list of meshes that are visible
        //Currently disabled while waiting for rework of model mesh loader and material system
        //Since we want frustum culling to work per mesh now, instead of per model.
        void frustrumCulling();
};

#endif