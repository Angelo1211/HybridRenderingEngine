#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-12
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO update comments
// ===============================

//Headers
#include "scene.h"

class SceneManager{
    public:
        //Dummy Constructor / Destructor
        SceneManager();
        ~SceneManager();

        //Initializes and closes all scene related stuff
        bool startUp();
        void shutDown();

        // Scene switching
        bool switchScene(std::string sceneID);

        // Update current scene
        void update(unsigned int deltaT);

        //Called by the rendermanager to prep the render queue 
        //Also called by the input manager as a precaution to avoid dangling pointers
        Scene* getCurrentScene();

    private:
        bool loadScene(std::string sceneID);

        //String could probably be an enum instead, but it's easier this way to build
        //the relative paths if it is a string.
        std::string currentSceneID;
        Scene* currentScene;
};

#endif