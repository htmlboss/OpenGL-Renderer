# OpenGL 3D Renderer

[![Open Source Love](https://badges.frapsoft.com/os/v1/open-source.png?v=103)](https://github.com/ellerbrock/open-source-badges/)
[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fd7023ebf2e941eeb7d4f67cf9c6ea1f)](https://www.codacy.com/app/htmlboss/OpenGL-Renderer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=htmlboss/OpenGL-Renderer&amp;utm_campaign=Badge_Grade)

[![forthebadge](http://forthebadge.com/images/badges/designed-in-ms-paint.svg)](http://forthebadge.com)
[![forthebadge](http://forthebadge.com/images/badges/powered-by-water.svg)](http://forthebadge.com)

## Screenshots
![Current Build](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/01.png "Current Build")

## Features
* Instancing.
* Skybox.
* Assimp model loading.
* Procedural terrain (first iteration).
* Post processing (HDR, saturation, vibrance).
* Frustum culling (AABB only).
* Forward shading.

## Coming "Soon"
* Post processing (bloom, lens flares).
* Rewritten deferred shading (w/ PBR).
* GPU terrain generation.
* Extended frustum culling:
    * Sphere and point testing.
    * AABB hierarchy.
    * Terrain AABB.

## Third Party Code
* Assimp.
* GLM.
* GLAD.
* GLFW.
* FreeType.
* stb.
* GLM AABB wrapper: ([https://github.com/iauns/cpm-glm-aabb](https://github.com/iauns/cpm-glm-aabb)).

## Research Used
*  _Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix_ ([http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf](http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf)). Gribb, Hartman (2001).