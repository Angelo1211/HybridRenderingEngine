/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-08
*/

#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

//Shader setup and initialization code, could be combined with the compute shader initialization and
//could also set success/failure flags to indicate issues during load to avoid full crashes. TODO
bool Shader::setup(const std::string vertexPath, const std::string fragmentPath, const std::string geometryPath){
    //Getting the vertex shader code from the text file at file path
    bool gShaderOn = geometryPath != "";
    std::string shaderFolderPath = "../assets/shaders/";

    std::string vertexCode, fragmentCode, geometryCode;
    std::stringstream vShaderStream, fShaderStream, gShaderStream;
    std::ifstream vShaderFile(shaderFolderPath + vertexPath),
                  fShaderFile(shaderFolderPath + fragmentPath),
                  gShaderFile(shaderFolderPath + geometryPath);

    //Check if shader files exist
    if(!vShaderFile.good()){
        printf("Couldn't find vertex shader file: %s in shaders folder.\n ", vertexPath.c_str());
        return false;
    }
    else{ //Vertex shader file exists
        if(!fShaderFile.good()){
            printf("Couldn't find fragment shader file: %s in shaders folder.\n ", fragmentPath.c_str());
            return false;
        }
        else{
            //Frgment shader file exists
            vShaderStream << vShaderFile.rdbuf();    
            fShaderStream << fShaderFile.rdbuf();    
            if( gShaderOn ) {gShaderStream << gShaderFile.rdbuf();}
            
            //Close Files
            vShaderFile.close();
            fShaderFile.close();
            gShaderFile.close();

            //Passing code from string stream to string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();
            const char* gShaderCode;

            if( gShaderOn ){
                geometryCode = gShaderStream.str();
                gShaderCode = geometryCode.c_str();
            }

            //Vertex shader stuff
            int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vShaderCode, NULL);
            glCompileShader(vertexShader);
            int success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if(!success){
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                printf("Vertex shader compilation failed %s\n", infoLog );
                return false;
            }

            //Fragment shader stuff
            int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
            glCompileShader(fragmentShader);
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if(!success){
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                printf("Fragment shader compilation failed %s\n", infoLog );
                return false;
            }

            //Geometry shader stuff
            int geometryShader;
            if (gShaderOn){
                geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(geometryShader, 1, &gShaderCode, NULL);
                glCompileShader(geometryShader);
                glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
                if(!success){
                    glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
                    printf("Geometry shader compilation failed %s\n", infoLog );
                    return false;
                }
            }

            //Linking shaders
            ID = glCreateProgram();
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, fragmentShader);
            if (gShaderOn) {glAttachShader(ID, geometryShader);}
            glLinkProgram(ID);

            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if(!success){
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                printf("Shader Linking failed %s\n", infoLog );
                return false;
            }

            //Deleting shaders
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            if (gShaderOn) {glDeleteShader(geometryShader);}

            return true;
        }
    } 
} 

//Indicate to openGL that this is the GPU program that is going to be run
void Shader::use() const {
    glUseProgram(ID);
}

//Setting uniforms within the shader, share functionality with compute
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID,name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int  value) const {
    glUniform1i(glGetUniformLocation(ID,name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID,name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
   glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
}

//--------------------------------------------------------------------------------------------
//The constructor is actually really similar to the base class one,
//Could be simplified?
bool ComputeShader::setup(const std::string computePath){
    //Getting the compute shader code from the text file at file path
    std::string shaderFolderPath = "../assets/shaders/ComputeShaders/";
    std::string computeCode;
    std::ifstream cShaderFile(shaderFolderPath + computePath);
    std::stringstream cShaderStream;
    //Check if shader files exist
    if(!cShaderFile.good()){
        printf("Couldn't find compute shader file: %s in shaders folder.\n ", computePath.c_str());
        return false;
    }
    else{ //Compute Shader Exists
        cShaderStream << cShaderFile.rdbuf();

        //Close Files
        cShaderFile.close();

        //Passing code from string stream to string
        computeCode = cShaderStream.str();
        const char *cShaderCode = computeCode.c_str();
        
        //OpenGL initialization
        int computeShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(computeShader, 1, &cShaderCode, NULL);
        glCompileShader(computeShader);
        int success;
        char infoLog[512];
        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
            printf("Vertex shader compilation failed %s\n", infoLog);
            return false;
        }

        //Linking shaders
        ID = glCreateProgram();
        glAttachShader(ID, computeShader);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success){
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            printf("Shader Linking failed %s\n", infoLog);
            return false;
        }

        //Deleting shaders
        glDeleteShader(computeShader);
        return true;
    } 
}

//Shorthand for dispatch compute with some default parameter values
void ComputeShader::dispatch(unsigned int x, unsigned int y, unsigned int z) const{
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
