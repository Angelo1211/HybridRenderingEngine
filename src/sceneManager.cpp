/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-12
*/

//Headers
#include "sceneManager.h"

//Dummy constructors / destructors
SceneManager::SceneManager(){}
SceneManager::~SceneManager(){}

//Starts up the scene manager and loads the default scene. If for whatever reason
// the scene could not load any model, or there are none defined it quits early.
bool SceneManager::startUp(){
    // currentSceneID = "pbrTest";
    currentSceneID = "Sponza";
    if (!loadScene(currentSceneID)){
        printf("Could not load default sponza scene. No models succesfully loaded!\n");
        return false;
    }
    return true;
}

void SceneManager::shutDown(){
    delete currentScene;
}

//Checks if the scene that you want to load is not the one that is currently loaded.
// If it isn't, then it deletes the current one and loads the new one.
bool SceneManager::switchScene(std::string newSceneID){
    if( newSceneID != currentSceneID ){
        currentSceneID = newSceneID;
        delete currentScene;
        return loadScene(newSceneID);
    }
    else{
        printf("Selected already loaded scene.\n");
        return true;
    }
}

//Misdirection towards the current scene to avoid pointer dangling after scene switching
void SceneManager::update(unsigned int deltaT){
    currentScene->update(deltaT);
}

Scene* SceneManager::getCurrentScene(){
    return currentScene;
}

//Loads the scene with the given ID. If the scene is empty it will declare it an unsuccesful
//load and attempt to quit early of the whole program
bool SceneManager::loadScene(std::string sceneID){
    currentScene = new Scene(sceneID);
    return  !currentScene->loadingError; //True if empty, so it's negated for startup
}