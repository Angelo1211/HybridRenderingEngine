#ifndef ENGINE_H
#define ENGINE_H

// ===============================
// AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
// CREATE DATE  : 2018-09-05
// PURPOSE      : TODO
// ===============================
// SPECIAL NOTES: TODO
// ===============================

//Headers
#include "displayManager.h"
#include "inputManager.h"
#include "sceneManager.h"
// #include "renderManager.h"


class Engine
{
  public:
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

  private:
    DisplayManager gDisplayManager;
    InputManager gInputManager;
    SceneManager gSceneManager;
    // RenderManager gRenderManager;
};

#endif