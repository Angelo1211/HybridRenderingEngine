# HRE :: Hybrid (Clustered Forward/Deferred) Rendering Engine 

[![GitHub license](https://img.shields.io/github/license/Angelo1211/HybridRenderingEngine.svg)](https://github.com/Angelo1211/HybridRenderingEngine/blob/master/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/Angelo1211/HybridRenderingEngine.svg)](https://github.com/Angelo1211/HybridRenderingEngine/issues)

HRE was a real-time, Physically based, Clustered renderer built with OpenGL and based on the techniques described on the paper [Clustered Deferred and Forward shading](http://www.cse.chalmers.se/~uffe/clustered_shading_preprint.pdf) by Ola Olsson, Markus Billeter and Ulf Assarsson. The aim of this project is to build a testing framework to showcase deferred and forward graphics techniques (hence the hybrid part) and to examine the strengths of Clustered shading. I wrote a lengthy blog post full of cool animations that can serve as an introduction to efficient rendering algorithms as well as a detailed guide of my implementation of Clustered Shading, so make sure to check it out [here!](http://www.aortiz.me/2018/12/21/CG.html)

This algorithm was selected for its two major benefits over traditional deferred/tiled solutions: it allows for more freedom to choose forward or deferred shading paths as desired and is a better long term fit for the modern GPU performance trends. It has already been succesfully implemented in some of the best looking games of the last couple of years, with some notable examples being: [DOOM 2016](https://www.slideshare.net/TiagoAlexSousa/siggraph2016-the-devil-is-in-the-details-idtech-666), [Just Cause 3](http://www.humus.name/Articles/PracticalClusteredShading.pdf) and [Detroit: Become Human](https://www.gdcvault.com/play/1025420/Cluster-Forward-Rendering-and-Anti). 

### Project is now on hold :(

Unfortunately this project is no longer maintained. I got hired to work at Rockstar Games as a Graphics Programmer shortly after posting this and don't have the time to work on a sideproject of this size anymore. I plan on keeping it up (as well as my other major graphics project [SSGE](https://github.com/Angelo1211/SoftwareRenderer)) so others can see what it took me to break into the industry. I will still be helping anyone who runs into any issues with it or has any questions about it. Feel free to reach me here or at [my twitter](https://twitter.com/aortizelguero) about this or any other GFX topic you'd like to talk about! Thanks to everyone who checked this project out! :D 

## Download

Stable builds can be found on the [HRE releases](https://github.com/Angelo1211/HybridRenderingEngine/releases) page. 

Currently only Windows platforms are supported.

## Render Samples
![helmetcrop](https://user-images.githubusercontent.com/11263073/49331372-01e09980-f59c-11e8-93af-c706c0571fb4.PNG)

![waterbottlefixed](https://user-images.githubusercontent.com/11263073/49331375-01e09980-f59c-11e8-9b62-34c7b8e5a986.PNG)

![metallicspheres](https://user-images.githubusercontent.com/11263073/49331373-01e09980-f59c-11e8-87c1-63c320ec9e3b.PNG)

![sponza](https://user-images.githubusercontent.com/11263073/49331374-01e09980-f59c-11e8-9306-beaec08e8ad6.PNG)
![correct colors](https://user-images.githubusercontent.com/11263073/49331468-ad3e1e00-f59d-11e8-9a1a-86813ea4f85e.PNG)

<center> <h4>Cluster z slices</h4> </center>

## Main Features

### Rendering 

* Clustered Forward/Deferred Renderer
* Physically Based shading
* Image Based Lighting
* Metallic workflow
* Cook-Torrance specular BRDF (w/ lambert diffuse)
* Ambient Occlusion & Emissive mapping
* Tangent space normal mapping
* HDR/linear lighting
* HDR/LDR skyboxes
* Exposure based tone mapping
* Bloom
* Multisample Anti-aliasing (MSAA)
* Directional & point light sources
* Compute shader based light culling
* Alpha tested foliage (sponza atrium)
* Directional light dynamic shadow mapping
* Static Omnidirectional Shadow Mapping for Point Lights
* Forward, Tiled Forward and Deferred rendering implementations
    * Located in other branches of the repo. Only clustered rendering is actively being developed.

### Engine
* OpenGL 4.5+
* SDL2 backend 
* JSON Parsing via Nlohmann: JSON for C++
* Model Loading via ASSIMP (OBJ, FBX, gLTF2.0 etc)
* Scene contents clearly outlined in JSON file
* Multiple image loading paths via stb-image and GLI
* Immediate mode GUI for debugging via ImGUI
* Fully commented for future referencing
* Environment map generation on load for IBL and skyboxes
* Free flight camera

## Development Timeline & Planned Features 

### Alas, I am unfortunately unable to work on this project anymore and won't be updating it any time soon. I am still keeping this section in here as a record in case I ever do return and as proof of my naive optimism. Thanks to everyone who checked out this project! :) 

The first development cycle began September 2018 and concluded December 2018 with the inclusion of Image Based Lighting and the release of the first stable version: HRE 0.01 "Aachen". For the next couple of months I'll be tackling the issues outlined in the [Projects section](https://github.com/Angelo1211/HybridRenderingEngine/projects) beginning with fixes for the remaining non-critical bugs and then moving on to a major re-write of the material system. The following is a a list of the engine features I aim to include over the coming months, it's obviously subject to change and all that stuff. 

#### Update #1: Materials & Shadow Mapping
 * Full material system re-write to decouple mesh & textures.
 * Implementing a shadow mapping atlas and automatic shadow map reuse.
 * Fix some unneccessary STL calls.

#### Update #2: Deferred / Screen Space Effects
* Rewriting screen space fragment shaders as compute shaders
* Screen Space Ambient Occlusion
* Screen Space Reflections
* Temporal AA
* Improved Color Grading & Tone Mapping

#### Update #3: Cluster Optimizations
* Large amounts of lights via BVH construction
* Advanced environment mapping
* View Frustum culling
* Compute shader based particles
* Include Approaching Zero Driver Overhead (AZDO) principles 

I'll be writing accompanying blog posts at the end of each major milestone, outlining the new features and their implementations and any complications I found along the way. Meanwhile, [Here's the first post](http://www.aortiz.me/2018/12/21/CG.html) which contains an overview of other traditional rendering algorithms and builds the case for Clustered rendering. 

## Repository structure

```bash
Hybrid Rendering Engine
|-- assets              
|   |-- models              # Meshes and textures
|   |-- scenes              # Scene description files in JSON format
|   |-- shaders             
|   |   |-- computeShaders  # Compute shaders for clustered forward
|   |   |-- OldShaders      # Shaders not currently in use
|   |   `-- currentShaders  # Shaders for the current rendering pipeline
|   `-- skyboxes            # Skyboxes in either cubemap or equirectangular map form
|-- build                   # CMake compile  
|-- include                 # Project header files
|-- libs                    # Check dependencies section for descriptions & links
|   |-- ASSIMP               
|   |-- Dear imgui           
|   |-- Glad 
|   |-- Json c++            
|   |-- GLI 
|   |-- GLM 
|   |-- stb_image 
|   `-- SDL2 
|-- modules                 # CMake lib setup scripts
|-- src                     # C++ implementation files
`-- CMakeLists.txt          # CMake build script
```

## Dependencies 

In my previous project, [SSGE](https://github.com/Angelo1211/SoftwareRenderer), I made an effort to reduce the amount of dependencies to a minimum. This time however, the pendulum has swung the other way and I've made no restriction whatsoever (within reason) as to what libraries I could include in the project. I did this because I wanted to get more comfortable with the process of including and working with external libraries in C++.

Here's a list of the libraries currently included in the project:

* [ASSIMP](https://github.com/assimp/assimp): Mesh and material loading.
* [dear imgui,](https://github.com/ocornut/imgui): Immediate Mode GUI.
* [glad](https://github.com/Dav1dde/glad): GL loader-generator.
* [JSON for Modern C++](https://github.com/nlohmann/json): Scene description and serialization.
* [OpenGL Image (GLI)](https://github.com/g-truc/gli): Image loading for DDS files.
* [OpenGL Mathematics (GLM)](https://glm.g-truc.net/0.9.9/index.html): Linear algebra / matrix library.
* [SDL2](https://www.libsdl.org/download-2.0.php): Hardware abstraction layer.
* [stb_image](https://github.com/nothings/stb): Image loading/decoding for JPG, PNG, HDR.

Here's a list of the libraries **not** included:

* [CMake](https://github.com/Kitware/CMake): Build generation.
* [OpenGL](https://www.opengl.org/): All things GPU & graphics.

All included libraries are represented here under their respective licenses which can be found in the links above or in the licenses.txt doc in the libs folder.

## References

There's a comprehensive indexed list of references for the project that I keep more or less up to date in this [page of the wiki](https://github.com/Angelo1211/HybridRenderingEngine/wiki/References). However, I find myself constantly returning to a subset of them for further reading or when I need inspiration, so I've added links below to share them and spread the word of their awesomeness. Check them out! 

* [Learn OpenGL](https://learnopengl.com/Introduction): OpenGL tutorials and general introduction to 3D graphics concepts.
* [Parallel Computer Architecture and Programming](http://15418.courses.cs.cmu.edu/tsinghua2017/home) : Intro to the GPU programming model. 
* [Doom(2016) - Graphics Study](http://www.adriancourreges.com/blog/2016/09/09/doom-2016-graphics-study/): Dissection of a frame in the id Tech 6 engine.
* [Siggraph2016 - The Devil is in the Details: idTech 666](https://www.slideshare.net/TiagoAlexSousa/siggraph2016-the-devil-is-in-the-details-idtech-666?next_slideshow=1): A behind-the scenes look into the renderer tech of DOOM 2016. Probably the biggest inspiration in terms of features and overall quality level that I'm striving for.  
* [Efficient Real-Time Shading with Many Lights](https://www.zora.uzh.ch/id/eprint/107598/1/a11-olsson.pdf): An introduction to clustered shading directly from the writers of the original paper. Also outlines some practical tips from industry veterans.
* [Real Shading in Unreal Engine 4](https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf): The principles and implementation details behind UE4 physically based shading model. Includes shader code!
* [Forward vs Deferred vs Forward+ Rendering with DirectX 11](https://www.3dgep.com/forward-plus/): Detailed overview of different rendering algorithms.

## Acknowledgements
### Models & Textures

* [Sponza (gLTF2 Version)](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Sponza)
: Original model: [Frank Meinl](https://www.artstation.com/artwork/K5bEr), First modified by: [Morgan McGuire](http://casual-effects.com/data/index.html), PBR Textures: [Alexandre-pestana](http://www.alexandre-pestana.com/pbr-textures-sponza/).
* [Barcelona Sunrise (HDR Map)](http://www.hdrlabs.com/sibl/archive.html): by [@Blochi](https://twitter.com/Blochi)
* [Battle Damaged Sci-fi Helmet (PBR)](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/DamagedHelmet): by [@theblueturtle_](https://sketchfab.com/theblueturtle_)
* [Metal Rough Test Spheres (PBR)](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/MetalRoughSpheres): by Ed Mackey.
* [gLTF Sample Model Repository](https://github.com/KhronosGroup/glTF-Sample-Models): All other models & textures.
* [sIBL Archive](http://www.hdrlabs.com/sibl/archive.html): All other HDR maps & skyboxes.
* [Default Skybox](http://www.custommapmakers.org/skyboxes.php)

Thank you to all the 3D & texture artists who have made their assets free to use and so beautiful to look at. If it weren't for you guys I'd probably still be staring at the same old boring utah teapot all damn day!  

If I've mis-acknowledged, misattributed, or missed any references, please, create a new GitHub issue or send me a DM on Twitter and I'll make sure to fix it immediately.

### Graphics Programming Community <3

Shout-out to Kostas Anagnostou and Eric Arnebäck for taking the time to answer all of my questions for the last couple of months, your explanations made understanding every daunting topic so much easier!

## License

MIT License

Copyright (c) 2018 Angel Ortiz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


