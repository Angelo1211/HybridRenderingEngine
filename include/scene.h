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
#include <queue>
#include "model.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"
// #include "light.h"

using json = nlohmann::json;

class Scene{
    public:
        //Builds scene using path to folder containing content and txt setup file
        Scene(const std::string &sceneFolder);
        ~Scene();

        //Updates all models, lights and cameras
        void update(unsigned int deltaT);

        //Getters used in the setup of the render queue
        std::queue<Model*>* getVisiblemodels();
        Camera * getCurrentCamera();
        // BaseLight * getCurrentLights();
        int getLightCount();
        
        //Signals issues to scene Manager
        bool checkIfEmpty();  

    private:
        std::string sceneID;
        const std::string folderPath = "../assets/scenes/";
        const std::string fileExtension = ".json";

        bool missingScene;
        int lightCount;
        Camera mainCamera;
        // BaseLight *lights; //Array of lights in scene

        //Contains the models that remain after frustrum culling
        std::queue<Model*> visibleModels;
        std::vector<Model*> modelsInScene;

        //TODO 
        bool checkFileValidity(const std::string &filePath);
        bool loadContent();
        void loadSceneModels(const json sceneConfigJson);
        
        //Finds objects that the camera can see
        void frustrumCulling();
};

#endif