# OpenGL 3D Renderer

## Intro
I've been extremely interested in computer graphics for a number of years now, but I've only possessed the C++ skill and math knowledge for just over a year to fully understand the theory behind DirectX/OpenGL. So this is my first implementation of a 3D Renderer in OpenGL using various websites as references to help me along ([Learn OpenGL](http://www.learnopengl.com/), [open.gl](http://open.gl), and naturally YouTube).
Obviously, performance is atrocious since my main goal here was to get visual results on-screen and learn to debug graphical run-time errors (and yes, those were painful), but I learned a ton of things about graphics and some finer nuances of C++ (and even contributed a [bugfix](https://github.com/matus-chochlik/oglplus/issues/135) for Windows to OGLPlus).
I'm presently reading through (albeit very slowly due to high course load) *Computer Graphics: Principles & Practice* and Nvidia's GPU Gems books (as I can get my hands on them) to really understand the architecture of a rendering system and the how's and why's of some de-facto designs.

## Features
*[Master Branch](https://github.com/htmlboss/OpenGL-Shader-Viewer/tree/master)*
* Instancing
* Geometry shaders
* HDR
* Reflection/specular maps
* Cubemaps
* Assimp model loading
* Font renderer (primarily used for displaying FPS since it's not optimized very much)

*[oglplus-testing Branch](https://github.com/htmlboss/OpenGL-Shader-Viewer/tree/oglplus-testing)*
* OO implementation of OpenGL (85% done)
* Gamma Correction
* Deferred shading
* Partial renderer abstraction (not finished)
* Bad performance :P

## Now what?
Looking forward, I've started to think I may have to start from scratch again so that I may implement something that is extensible to me, but doesn't become a performance black-hole. You can see from the code yourself, there are a lot of design problems, and there exists a nasty mash between procedural and OO code (these 2 things can be used together, but I've just done it poorly). Some things I want to learn (and hopefully implement before I die from old-age):
* Scenegraph
* Proper rendering abstraction (so I can write a DirectX backend in the future and use it interchangeably)
* Terrain rendering (shader splatting like in Frostbite), and terrain nodes.
* PBR (yup...I want to get on that hype train too)
* Realtime global Illumination (GI) using *some* method (photon mapping, voxel-based, ray marching, etc)
* Dynamic skyboxes
* Realistic water rendering
* Advanced post-processing (basically most of the GPU Gems stuff)
* Volumetric everything (lighting, fog, clouds, rain)
* LENS FLARES!!!!!
