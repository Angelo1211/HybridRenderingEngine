#include "fileManager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>

bool FLOAD::checkFileValidity(const std::string &filePath){
    struct stat info;
    //file is blocking access
    if( stat( filePath.c_str(), &info ) != 0 ){
        printf( "Cannot access %s\n", filePath.c_str() );
         return false;
    }
    else if( info.st_mode & S_IFMT ){
        //file is accessible
        printf( "%s is a valid file\n", filePath.c_str() );
        return true;
    }
    else{
        //File does not exist
        printf("Error! File: %s does not exist.\n", filePath.c_str());
        return false;
    }
}