# MP-APS
A modern OpenGL 3D rendering engine.

![MPAPS](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/MP-APS.png "MPAPS")

[![Open Source Love](https://badges.frapsoft.com/os/v1/open-source.png?v=103)](https://github.com/ellerbrock/open-source-badges/)
[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fd7023ebf2e941eeb7d4f67cf9c6ea1f)](https://www.codacy.com/app/htmlboss/OpenGL-Renderer?utm_source=github.com&utm_medium=referral&utm_content=htmlboss/OpenGL-Renderer&utm_campaign=badger)

[![forthebadge](http://forthebadge.com/images/badges/designed-in-ms-paint.svg)](http://forthebadge.com)
[![forthebadge](http://forthebadge.com/images/badges/powered-by-water.svg)](http://forthebadge.com)


## Screenshots
CPU-based terrain generation using Perlin noise:
![Terrain](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/01.png "Terrain")

Anti-aliased wireframe rendering:
![Wireframe rendering](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/02.png "Wireframe rendering")

Physically Based Rendering:
![PBR](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/03.png "Physically Based Rendering")
![PBR](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/04.png "Physically Based Rendering")

## Features
* Assimp model loading.
* CPU-based procedural terrain (first iteration).
* Post processing (HDR, saturation, vibrance).
* Parallel AABB frustum culling.
* XML engine configuration.
* Shader-based wireframe overlay.
* Physically-based rendering.

## WIP
* SMAA

## Third Party Code
* Assimp.
* GLM.
* GLAD.
* GLFW.
* stb_image.
* pugixml.
* GLM AABB wrapper: ([https://github.com/iauns/cpm-glm-aabb](https://github.com/iauns/cpm-glm-aabb)).

## Research Used
*  _Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix_ ([http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf](http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf)). Gribb, Hartman (2001).
* _Forward+: Bringing Deferred Lighting to the Next Level_ ([https://takahiroharada.files.wordpress.com/2015/04/forward_plus.pdf](https://takahiroharada.files.wordpress.com/2015/04/forward_plus.pdf)). Harada, McKee, Yang (2012).
* _Solid Wireframe_ ([http://developer.download.nvidia.com/SDK/10/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf](http://developer.download.nvidia.com/SDK/10/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf)). Nvidia (2007).
* _Physically Based Rendering_ ([https://learnopengl.com/#!PBR/IBL/Specular-IBL](https://learnopengl.com/#!PBR/IBL/Specular-IBL))