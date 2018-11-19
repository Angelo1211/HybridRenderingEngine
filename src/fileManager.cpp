/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-19
*/

//Header files
#include "fileManager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>

//Checks if a file exits and can be read
bool FLOAD::checkFileValidity(const std::string &filePath){
    struct stat info;
    //file is blocking access or read 
    if( stat( filePath.c_str(), &info ) != 0 ){
        printf( "Cannot access %s\n", filePath.c_str() );
         return false;
    }
    //file is accessible
    else if( info.st_mode & S_IFMT ){
        printf( "%s is a valid file\n", filePath.c_str() );
        return true;
    }
    //File does not exist
    else{
        printf("Error! File: %s does not exist.\n", filePath.c_str());
        return false;
    }
}

 //Get index location of last dot. Return empty sentinel if no dot can be found
std::string FLOAD::getFileExtension(const std::string &filePath){
    size_t indexLocation = filePath.rfind('.', filePath.length());
    if( indexLocation != std::string::npos){
       return  filePath.substr(indexLocation + 1, filePath.length() - indexLocation);
    }
    return "";
}