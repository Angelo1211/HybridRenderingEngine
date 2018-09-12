// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-08
// ===============================

#include "shader.h"
#include "SDL.h"
#include <fstream>
#include <iostream>
#include <sstream>

/*
TODO Make this a fully error checked class and return to the above levels info on where
it crashed and burned  
*/

Shader::Shader(const std::string vertexPath, const std::string fragmentPath){
    //Getting the vertex shader code from the text file at file path
    std::string shaderFolderPath = "../assets/shaders/";
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile(shaderFolderPath + vertexPath), fShaderFile(shaderFolderPath + fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    //Check if shader files exist
    if(!vShaderFile.good()){
        printf("Couldn't find vertex shader file: %s in shaders folder.\n ", vertexPath.c_str());
        SDL_Delay(3000);
    }
    else{ //Vertex shader file exists
        if(!fShaderFile.good()){
            printf("Couldn't find fragment shader file: %s in shaders folder.\n ", vertexPath.c_str());
            SDL_Delay(3000);
        }
        else{ //Frgment shader file exists
            vShaderStream << vShaderFile.rdbuf();    
            fShaderStream << fShaderFile.rdbuf();    
            
            //Close Files
            vShaderFile.close();
            fShaderFile.close();

            //Passing code from string stream to string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

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
            }

            //Fragment shader stuff
            int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
            glCompileShader(fragmentShader);
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if(!success){
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                printf("Fragment shader compilation failed %s\n", infoLog );
            }

            //Linking shaders
            ID = glCreateProgram();
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, fragmentShader);
            glLinkProgram(ID);

            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if(!success){
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                printf("Shader Linking failed %s\n", infoLog );
            }

            //Deleting shaders
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }
    } 
} 

void Shader::use(){
    glUseProgram(ID);
}

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
