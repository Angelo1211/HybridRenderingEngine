# HRE :: Hybrid (Clustered Forward/Deferred) Rendering Engine 

## Download

## Render Samples

## Main Features

### Rendering 

### Engine

## Development Timeline & Planned Features

## Repository structure

```bash
Hybrid Rendering Engine
|-- assets              
|   |-- models              # Meshes and textures
|   |-- scenes              # Scene description files in JSON format
|   |-- shaders             
|   |   |-- computeShaders  # Compute shaders for cluster forward
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
`-- CMakeLists.txt          # CMake build scripts
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

All libraries are included under their respective licenses which can be referred to at the links above.

## References

There's a comprehensive indexed list of references for the project that I am continually updating in this [page of the wiki](). However, I am constantly returning to a subset of these for reference and/or inspiration so I added links to them below to highlight them and share their awesomeness: 

* [Learn OpenGL](https://learnopengl.com/Introduction): OpenGL tutorials and general introduction to 3D graphics concepts.
* [Parallel Computer Architecture and Programming](http://15418.courses.cs.cmu.edu/tsinghua2017/home) : Intro to the GPU programming model. 
* [Siggraph2016 - The Devil is in the Details: idTech 666](https://www.slideshare.net/TiagoAlexSousa/siggraph2016-the-devil-is-in-the-details-idtech-666?next_slideshow=1): Main guideline and inspiration for features of the HRE. 
* [Efficient Real-Time Shading with Many Lights](https://www.zora.uzh.ch/id/eprint/107598/1/a11-olsson.pdf): Great explanation of cluster shading directly from the writers of the original paper and practical tips from industry veterans who have implemented it.

## Model & Textures Acknowledgements

* [Sponza (gLTF2 Version)](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Sponza)
: Original model: [Frank Meinl](https://www.artstation.com/artwork/K5bEr), First modified by: [Morgan McGuire](http://casual-effects.com/data/index.html), PBR Textures: [Alexandre-pestana](http://www.alexandre-pestana.com/pbr-textures-sponza/).
* [Barcelona Sunrise (HDR Map)](http://www.hdrlabs.com/sibl/archive.html): by [@Blochi](https://twitter.com/Blochi)
* [Battle Damaged Sci-fi Helmet (PBR)](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/DamagedHelmet): by [@theblueturtle_](https://sketchfab.com/theblueturtle_)
* [Metal Rough Test Spheres (PBR)](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/MetalRoughSpheres): by Ed Mackey.
* [gLTF Sample Model Repository](https://github.com/KhronosGroup/glTF-Sample-Models): All other models & textures.
* [sIBL Archive](http://www.hdrlabs.com/sibl/archive.html): All other HDR maps & skyboxes.
* [Default Skybox](http://www.custommapmakers.org/skyboxes.php)

Thank you to all the 3D & texture artists who have made their assets free to use and so awesome to look at. If it weren't for you guys I'd probably still be staring at the same old boring utah teapot all damn day!  

If I've mis-acknowledged, misattributed, or missed any references, please, create a new GitHub issue or send me a DM on Twitter and I'll make sure to fix it immediately.


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


