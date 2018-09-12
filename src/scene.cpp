// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-07-10
// ===============================

//Headers
#include "scene.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "SDL.h"

using json = nlohmann::json;
// #include <fstream>
// #include <sstream>
Scene::Scene(const std::string &sceneName){
    sceneID = sceneName;
    if( !findSceneFile(folderPath + sceneName + fileExtension) ){
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
        // delete [] lights;
    }
}

//Update Order is critical for correct culling
void Scene::update(unsigned int deltaT){
    mainCamera.update(deltaT);
    // for(int i=0; i < lightCount; ++i){
    //     lights[i].update(deltaT);
    // }
    // for(Model *model : modelsInScene){
    //     model->update();
    // }
    // frustrumCulling();
}
//-----------------------------GETTERS----------------------------------------------
std::queue<Model*>* Scene::getVisiblemodels(){
    return &visibleModels;
}
Camera* Scene::getCurrentCamera(){
    return &mainCamera;
}
// BaseLight * Scene::getCurrentLights(){
//     return lights;
// }
int Scene::getLightCount(){
    return lightCount;
}
//----------------------------------------------------------------

bool Scene::checkIfEmpty(){
    return missingScene;
}

//-----------------------------SCENE LOADING-----------------------------------

//Config file parsing, gets all the important 
bool Scene::loadContent(){
    std::string sceneConfigFilePath = folderPath + sceneID + fileExtension;
    std::ifstream file(sceneConfigFilePath.c_str());
    // TransformParameters initParameters;

    //Begin config file parsing
    if( !file.good() ){
        //Check config file exists
        printf("Error! Config: %s does not exist.\n", sceneConfigFilePath.c_str());
        return false;
    }
    else{
        //Parsing into Json file readable format
        json j;
        file >> j;
        //Checking that config file belongs to current scene and is properly formatted
        // std::string line, key, x, y, z;
        // std::getline(file,line);
        // std::istringstream iss(line);
        // iss >> key;
        // if(key != "s"){
        //     printf("Error! Config file: %s is not properly formatted.\n",configFilePath.c_str());
        //     return false;
        // }
        // else{
        //     iss >> key;
        //     if(key != sceneName){
        //         //Checks the config file belongs to the correct scene
        //         printf("Error! Config file: %s does not belong to current scene.\n",configFilePath.c_str());
        //         return false;
        //     }
        //     else{
        //         //Now we can parse the rest of the file "safely"
        //         while(!file.eof()){
        //             std::getline(file,line);
        //             std::istringstream iss(line);
        //             iss >> key;
        //             //MODEL SETUP
        //             if(key == "m"){ 
        //                 printf("Loading models...\n");
        //                 iss >> key;
        //                 int modelCount = stoi(key);
        //                 std::string modelMeshID, modelMaterialID;
        //                 for(int i = 0; i < modelCount; ++i){

        //                     //Get model mesh and material info
        //                     std::getline(file,line);
        //                     std::istringstream modelData(line);
        //                     modelData >> key >> modelMeshID >> modelMaterialID;

        //                     //Position
        //                     std::getline(file,line);
        //                     std::istringstream pos(line);
        //                     pos >> key >> x >> y >> z;
        //                     initParameters.translation = Vector3f(stof(x), stof(y), stof(z));

        //                     //Rotation
        //                     std::getline(file,line);
        //                     std::istringstream rot(line);
        //                     rot >> key >> x >> y >> z;
        //                     initParameters.rotation = Vector3f(stof(x)*M_PI/180.0f, stof(y)*M_PI/180.0f, stof(z)*M_PI/180.0f);

        //                     //Scaling
        //                     std::getline(file,line);
        //                     std::istringstream sca(line);
        //                     sca >> key >> x >> y >> z;
        //                     initParameters.scaling = Vector3f(stof(x), stof(y), stof(z));

        //                     //Burning empty line that makes the config easier to read
        //                     std::getline(file,line);
                            
        //                     //Attempts to load model with the initparameters it has read
        //                     loadSceneModel(baseFilePath, initParameters, modelMeshID, modelMaterialID);
        //                 }
        //             }
        //             //LIGHT SETUP
        //             else if(key == "l"){ 
        //                 printf("Loading lights...\n");
        //                 iss >> key;
        //                 lightCount = stoi(key);
        //                 //Initializes light array
        //                 std::string lightType, radius, period;
        //                 lights = new BaseLight[lightCount];
        //                 for(int i = 0; i < lightCount; ++i){

        //                     //Obtain light type and depending on that get orbit or linear
        //                     std::getline(file,line);
        //                     std::istringstream lightData(line);
        //                     lightData >> key >> lightType;
        //                     if(lightType == "o"){
        //                         lights[i].type = 'o';
        //                         std::getline(file,line);
        //                         std::istringstream orb(line);
        //                         orb >> key >> radius >> period;
        //                         lights[i].radius = stof(radius);
        //                         lights[i].time /= stof(period)*1000; //miliseconds
        //                     }
        //                     else if(lightType == "l"){
        //                         lights[i].type = 'l';
        //                         std::getline(file,line);
        //                         std::istringstream orb(line);
        //                         orb >> key >> radius >> period;
        //                         lights[i].radius = stof(radius);
        //                         lights[i].time /= stof(period)*1000;
        //                     }
        //                     else if(lightType == "c"){
        //                         lights[i].type = 'c';
        //                     }
        //                     else if(lightType == "f"){
        //                         lights[i].type = 'f';
        //                     }

        //                     //Position
        //                     std::getline(file,line);
        //                     std::istringstream pos(line);
        //                     pos >> key >> x >> y >> z;
        //                     lights[i].position = Vector3f(stof(x), stof(y), stof(z));

        //                     //Color
        //                     std::getline(file,line);
        //                     std::istringstream col(line);
        //                     col >> key >> x >> y >> z;
        //                     lights[i].color = Vector3f(stof(x), stof(y), stof(z));

        //                     //Burning empty line that makes the config easier to read
        //                     std::getline(file,line);
        //                 }
        //             }
        //         }
        //Lastly we check if the scene is empty and return
        return !modelsInScene.empty();
    }
}

bool Scene::findSceneFile(const std::string &scenePath){
    struct stat info;
    //file is blocking access
    if( stat( scenePath.c_str(), &info ) != 0 ){
        printf( "cannot access %s\n", scenePath.c_str() );
         return false;
    }
    else if( info.st_mode & S_IFMT ){
        //file is accessible
        printf( "%s is a valid scene\n", scenePath.c_str() );
        return true;
    }
    else{
        //File does not exist
        printf("Error! Scene: %s does not exist.\n",scenePath.c_str());
        return false;
    }
}

// void Scene::loadSceneModel(const std::string &baseFilePath, const TransformParameters &init,const std::string modelMeshID, const std::string modelMaterialID){
//     std::string meshFilePath = baseFilePath + "/meshes/" + modelMeshID + "_mesh.obj";
//     if(!OBJ::fileExists(meshFilePath)){
//         //If the mesh deos not exist it's very likely nothing else does, quit early
//         printf("Error! Mesh: %s does not exist.\n",meshFilePath.c_str());
//     }
//     else{
//         printf( "%s is a valid mesh\n", meshFilePath.c_str() );
//         std::string materialPath = baseFilePath + "/materials/" + modelMaterialID + "/"  + modelMaterialID;
//         modelsInScene.push_back(new Model(init, meshFilePath, materialPath));
//     }
// }

//-------------------------------------------------------------

// void Scene::frustrumCulling(){
//     for(Model *model : modelsInScene){
//         bool visible = mainCamera.checkVisibility(model->getBounds());
//         if (visible) {
//             visibleModels.push(model);
//         }
//     }
// }












