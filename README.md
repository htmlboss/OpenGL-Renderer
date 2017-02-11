# OpenGL 3D Renderer

[![Open Source Love](https://badges.frapsoft.com/os/v1/open-source.png?v=103)](https://github.com/ellerbrock/open-source-badges/)
[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fd7023ebf2e941eeb7d4f67cf9c6ea1f)](https://www.codacy.com/app/htmlboss/OpenGL-Renderer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=htmlboss/OpenGL-Renderer&amp;utm_campaign=Badge_Grade)

[![forthebadge](http://forthebadge.com/images/badges/designed-in-ms-paint.svg)](http://forthebadge.com)

[![forthebadge](http://forthebadge.com/images/badges/powered-by-water.svg)](http://forthebadge.com)



## Intro
I've been extremely interested in computer graphics for a number of years now, but I've only possessed the C++ skill and math knowledge for just over a year to fully understand the theory behind DirectX/OpenGL. So this is my first implementation of a 3D Renderer in OpenGL using various websites as references to help me along ([Learn OpenGL](http://www.learnopengl.com/), [open.gl](http://open.gl), and naturally YouTube).
Obviously, performance is atrocious since my main goal here was to get visual results on-screen and learn to debug graphical run-time errors (and yes, those were painful), but I learned a ton of things about graphics and some finer nuances of C++ (and even contributed a [bugfix](https://github.com/matus-chochlik/oglplus/issues/135) for Windows to OGLPlus).
I'm presently reading through (albeit very slowly due to high course load) *Computer Graphics: Principles & Practice* and Nvidia's GPU Gems books (as I can get my hands on them) to really understand the architecture of a rendering system and the how's and why's of some de-facto designs.

## Features
* Instancing
* Cubemaps
* Skydome (UV map is broken right now)
* Assimp model loading
* Font renderer (primarily used for displaying FPS since it's not optimized very much)
* Deferred rendering

## Roadmap
* __WIP__: Reflections/refractions
* Bump/parallax mapping
* Shadowing
* Scenegraph
* __WIP__: Proper rendering abstraction (so I can write a DirectX backend in the future and use it interchangeably)
* Terrain rendering (shader splatting like in Frostbite), and terrain nodes.
* PBR.
* Realtime global Illumination (GI) using *some* method (photon mapping, voxel-based, ray marching, etc)
* Dynamic skyboxes
* Realistic water rendering
* Advanced post-processing (basically most of the GPU Gems stuff)
* Volumetric everything (lighting, fog, clouds, rain)
* LENS FLARES!!!!!

## Note:
I'm always keeping my eye out for performance improvements and fixing memory leaks, so I prefer to do things as correctly as possible ONCE instead of slapping something together and THEN optimizing later. Now I'm not following the bad practice of "premature optimization" by any means, but I just want to do things the RIGHT way.