#ifndef ENGINE_H
#define ENGINE_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	       : 2018-09-05
PURPOSE      : High level manager class that is in charge of the startup and shutdown of all
               major subsystems. Also contains the main execution loop of the program. 
SPECIAL NOTES: Built for educational purposes.
*/

//Headers
#include "displayManager.h"
#include "inputManager.h"
#include "sceneManager.h"
#include "renderManager.h"

struct Engine{
  //Dummy constructors / Destructors
  Engine();
  ~Engine();

  //I use these methods instead of constructors and destructors
  //because I want to be able to control initialization order.
  //You'll see the same idea applied to all manager level classes.
  bool startUp();
  void shutDown();

  //Contains all high level logic and the main application loop
  void run();

  //Instances of each major subsystem
  DisplayManager gDisplayManager;
  InputManager gInputManager;
  SceneManager gSceneManager;
  RenderManager gRenderManager;
};

#endif