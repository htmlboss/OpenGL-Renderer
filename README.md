# OpenGL 3D Renderer


[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fd7023ebf2e941eeb7d4f67cf9c6ea1f)](https://www.codacy.com/app/htmlboss/OpenGL-Renderer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=htmlboss/OpenGL-Renderer&amp;utm_campaign=Badge_Grade)

## Intro
I've been extremely interested in computer graphics for a number of years now, but I've only possessed the C++ skill and math knowledge for just over a year to fully understand the theory behind DirectX/OpenGL. So this is my first implementation of a 3D Renderer in OpenGL using various websites as references to help me along ([Learn OpenGL](http://www.learnopengl.com/), [open.gl](http://open.gl), and naturally YouTube).
Obviously, performance is atrocious since my main goal here was to get visual results on-screen and learn to debug graphical run-time errors (and yes, those were painful), but I learned a ton of things about graphics and some finer nuances of C++ (and even contributed a [bugfix](https://github.com/matus-chochlik/oglplus/issues/135) for Windows to OGLPlus).
I'm presently reading through (albeit very slowly due to high course load) *Computer Graphics: Principles & Practice* and Nvidia's GPU Gems books (as I can get my hands on them) to really understand the architecture of a rendering system and the how's and why's of some de-facto designs.

## Features
*[Master Branch](https://github.com/htmlboss/OpenGL-Shader-Viewer/tree/master)*
* Instancing
* Cubemaps
* Skydome (UV map is broken right now)
* Assimp model loading
* Font renderer (primarily used for displaying FPS since it's not optimized very much)

## Almost finished
* Deferred shading

## Roadmap
* Scenegraph
* Proper rendering abstraction (so I can write a DirectX backend in the future and use it interchangeably)
* Terrain rendering (shader splatting like in Frostbite), and terrain nodes.
* PBR.
* Realtime global Illumination (GI) using *some* method (photon mapping, voxel-based, ray marching, etc)
* Dynamic skyboxes
* Realistic water rendering
* Advanced post-processing (basically most of the GPU Gems stuff)
* Volumetric everything (lighting, fog, clouds, rain)
* LENS FLARES!!!!!
