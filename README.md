# HRE :: Hybrid (Clustered Forward/Deferred) Rendering Engine 


[![GitHub license](https://img.shields.io/github/license/Angelo1211/HybridRenderingEngine.svg)](https://github.com/Angelo1211/HybridRenderingEngine/blob/master/LICENSE)
[![GitHub last commit](https://img.shields.io/github/last-commit/google/skia.svg)](https://github.com/Angelo1211/HybridRenderingEngine)
[![GitHub issues](https://img.shields.io/github/issues/Angelo1211/HybridRenderingEngine.svg)](https://github.com/Angelo1211/HybridRenderingEngine/issues)


HRE is a real-time, Physically Based, Clustered renderer built with OpenGL and based on the technique described on the paper [Clustered Deferred and Forward shading](http://www.cse.chalmers.se/~uffe/clustered_shading_preprint.pdf) by Ola Olsson, Markus Billeter and Ulf Assarsson. Clustered shading offers two major benefits over traditional rendering algorithms: it allows for more freedom to choose forward or deferred shading paths as desired and is a better long term fit for the modern GPU performance trends, as GPU compute capabilities continue to outpace memory bandwidth growth. It has already been succesfully implemented in some of the best looking games of the last couple of years, with some notable examples being: [DOOM 2016](https://www.slideshare.net/TiagoAlexSousa/siggraph2016-the-devil-is-in-the-details-idtech-666), [Just Cause 3](http://www.humus.name/Articles/PracticalClusteredShading.pdf) and [Detroit: Become Human](https://www.gdcvault.com/play/1025420/Cluster-Forward-Rendering-and-Anti).

HRE is built with the sole aim of showcasing the strengths of this rendering algorithm and is currently in active development, with planned features outlined for the coming months.  

## Download

Stable builds can be found on the [HRE releases](https://github.com/Angelo1211/HybridRenderingEngine/releases) page. 

Currently only Windows platforms are supported.

## Render Samples

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
    * Located in other branches of the repo. Only clustered rendering is actively being developed

### Engine
* OpenGL 4.5+
* SDL2 backend 
* JSON Parsing via Nlohmann: JSON for C++
* Model Loading via ASSIMP (OBJ, FBX, gLTF2.0 etc)
* Scene contents outlined in JSON file
* Multiple image loading paths via stb-image and GLI
* Immediate mode GUI for debugging via ImGUI
* Fully commented for future referencing
* Environment map generation on load (for skyboxes)
* Free flight camera
* Windows Platform

## Development Timeline & Planned Features

The first development cycle began September 2018 and concluded late November 2018 with the inclusion of Image Based Lighting and the release of the first stable version: [HRE 0.01 "Aachen"](). For the next couple of months I'll be tackling the issues outlined in the [Projects section](https://github.com/Angelo1211/HybridRenderingEngine/projects) beginning with fixes for the remaining non-critical bugs and then moving on to a major re-write of the material system. The following is a a list of the engine features I aim to include over the coming months, it's obviously subject to change and all that stuff. 

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
* Approaching Zero Driver Overhead (AZDO)

I'll be writing accompanying blog posts at the end of each major milestone, outlining the new features and their implementations and any complications I found along the way.

Meanwhile, [Here's the first post]() which contains an overview of other traditional rendering algorithms and builds the case for Clustered rendering. 

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
|-- build                   
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

In my previous project, [SSGE](https://github.com/Angelo1211/SoftwareRenderer), I made an effort to reduce the amount of dependencies to a minimum. This time however, the pendulum has swung the other way and I've made no restriction whatsoever (within reason) as to what libraries I would include in the project. I did this because I wanted to get more comfortable with the process of including and working with external libraries in C++.

Here's a list of the libraries currently included in the project:

* [ASSIMP](https://github.com/assimp/assimp): Mesh and material loading.
* [CMake](https://github.com/Kitware/CMake): Build generation.
* [dear imgui,](https://github.com/ocornut/imgui): Immediate Mode GUI.
* [glad](https://github.com/Dav1dde/glad): GL loader-generator.
* [JSON for Modern C++](https://github.com/nlohmann/json): Scene description and serialization.
* [OpenGL](https://www.opengl.org/): All things GPU & graphics.
* [OpenGL Image (GLI)](https://github.com/g-truc/gli): Image loading for DDS files.
* [OpenGL Mathematics (GLM)](https://glm.g-truc.net/0.9.9/index.html): Linear algebra / matrix library.
* [SDL2](https://www.libsdl.org/download-2.0.php): Hardware abstraction layer.
* [stb_image](https://github.com/nothings/stb): Image loading/decoding for JPG, PNG, HDR.

All libraries are included under their respective licenses which can be found in the links above or in the licenses markdown document.

## References

There's a comprehensive indexed list of references for the project that I keep more or less up to date in this [page of the wiki](). However, I find myself constantly returning to a subset of them for further reading or when I need inspiration, so I've added links below to share them and spread the word of their awesomeness. Check them out! 

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

Shout-out to Kostas Anagnostou and Eric Arnebäck for taking the time to answer all of my questions for the last couple of months, your explanations made understanding every daunting topic a breeze!

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


