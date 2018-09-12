#ifndef SCENE_H
#define SCENE_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-10
// PURPOSE      : Contains all of the world information. The objects that you want to 
//                render, the camera that represents the viewer and the lights within
//                a scene. It also performs the view frustrum culling check to see which
//                objects should be visible by the camera at any given time and keeps
//                that list updated.
// ===============================
// SPECIAL NOTES: I use vectors here instead of arrays. Which I though would be necessary
// given that I would not know how many items would be loaded. It probably should be 
// changed in any future update to the engine.
// ===============================

//Headers
#include <vector>
#include <queue>
#include "model.h"
#include "camera.h"
// #include "light.h"

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
        bool findSceneFile(const std::string &scenePath);
        bool loadContent();
        // void loadSceneModel(const std::string &baseFilePath, const TransformParameters &init ,const std::string modelMeshID, const std::string modelMaterialID);
        
        //Finds objects that the camera can see
        // void frustrumCulling();
};

#endif