/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-24
*/

#include "skybox.h"
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

void Skybox::draw(){
    //We change the depth function since we set the skybox to a clipspace value of one
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxCubeMap.textureID);
    skyBoxCubeMap.drawCube();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

//Two setup paths for HDR and non HDR cubemap
void Skybox::setup(const std::string &skyboxName, bool isHDR, int res){
    std::string skyBoxFolderPath = "../assets/skyboxes/";
    skyBoxFolderPath += skyboxName;
    std::string skyBoxFilePath = skyBoxFolderPath + "/" + skyboxName + ".hdr";

    resolution = res;    

    //If the skybox is HDR it will come in as an equirectangular map,
    //We need to load it in and generate the cubemap that will be shown
    if(isHDR){
        equirectangularMap.loadHDRTexture(skyBoxFilePath);
        skyBoxCubeMap.generateCubeMap(res, res, HDR_MAP);
    }
    else{
        skyBoxCubeMap.loadCubeMap(skyBoxFolderPath);
    }
}

//Instead of passing the shader in, we could call this function when skybox is initialized?
void Skybox::fillCubeMapWithTexture(const Shader &buildCubeMapShader){
    skyBoxCubeMap.equiRectangularToCubeMap(equirectangularMap.textureID, resolution, buildCubeMapShader);
}