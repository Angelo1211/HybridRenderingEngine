#ifndef FILEMANAGER_H
#define FILEMANAGER_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-19
PURPOSE      : Abstract file loading and checking from scene and other implementation
               classes.
SPECIAL NOTES: Pretty bare bones, should look into which functions could be moved here. 
*/

//Header files
#include <string>

namespace FLOAD{
    bool checkFileValidity(const std::string &filePath);
    std::string getFileExtension(const std::string &filePath);
}

#endif 