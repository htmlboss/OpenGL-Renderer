# MP-APS
A modern OpenGL 3D rendering engine. The idea behind it is to be very simple to use: no weird abstraction layers, janky variable names, or massive 2000+ line classes. It's primarily a playground for me to implement (or at least try :P) different graphics programming techniques. Progress has been slow since I'm juggling full-time Univesity studies, work, and trying to be physically active. Eventually, I want to create a "scene-based" engine, where you can load up some model(s), add some fancy lights, and virtually explore it.

![MPAPS](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/MP-APS.png "MPAPS")

[![Open Source Love](https://badges.frapsoft.com/os/v1/open-source.png?v=103)](https://github.com/ellerbrock/open-source-badges/)
[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fd7023ebf2e941eeb7d4f67cf9c6ea1f)](https://www.codacy.com/app/htmlboss/OpenGL-Renderer?utm_source=github.com&utm_medium=referral&utm_content=htmlboss/OpenGL-Renderer&utm_campaign=badger)

[![forthebadge](http://forthebadge.com/images/badges/designed-in-ms-paint.svg)](http://forthebadge.com)
[![forthebadge](http://forthebadge.com/images/badges/powered-by-water.svg)](http://forthebadge.com)


## Compiling
### Ubuntu (and similar)

```sh
bash install-deps-ubuntu.sh
make
cd build/
./MP-APS
```

## Screenshots

Anti-aliased wireframe rendering:
![Wireframe rendering](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/02.png "Wireframe rendering")

Physically Based Rendering:
![PBR](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/03.png "Physically Based Rendering")
![PBR](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/04.png "Physically Based Rendering")

Image Based Lighting (IBL):
![IBL](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/06.png "Image Based Lighting (IBL)")

Bloom:
![Bloom](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/05.png "Bloom")

(OLD VERSION) CPU-based terrain generation using Perlin noise:
![Terrain](https://github.com/htmlboss/OpenGL-Renderer/blob/master/MP-APS/img/01.png "Terrain")

## Features
* Assimp model loading.
* Post processing (HDR, vibrance, bloom).
* Parallel AABB frustum culling.
* XML engine configuration.
* Support for `#include` directives in shaders.
* Shader-based wireframe overlay.
* Physically-based rendering.

## WIP
* Variance shadow mapping (soft shadows).
* Depth-buffer optimization ([https://outerra.blogspot.ca/2012/11/maximizing-depth-buffer-range-and.html](https://outerra.blogspot.ca/2012/11/maximizing-depth-buffer-range-and.html)).

## Roadmap
* Shadow volumes ([http://www.alexandre-pestana.com/volumetric-lights/](http://www.alexandre-pestana.com/volumetric-lights/), [https://www.slideshare.net/BenjaminGlatzel/volumetric-lighting-for-many-lights-in-lords-of-the-fallen](https://www.slideshare.net/BenjaminGlatzel/volumetric-lighting-for-many-lights-in-lords-of-the-fallen)).
* Per-material array textures:
  * ([https://www.gamedev.net/forums/topic/662654-best-practise-texture-atlas-38-vbo/?do=findComment&comment=5191703](https://www.gamedev.net/forums/topic/662654-best-practise-texture-atlas-38-vbo/?do=findComment&comment=5191703))
  * ([https://www.khronos.org/opengl/wiki/Array_Texture](https://www.khronos.org/opengl/wiki/Array_Texture))
* Re-implementing Forward+.
* More post-processing effects:
  * FXAA ([http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html](http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html)).
  * Chromatic aberration ([https://gamedev.stackexchange.com/a/58412/74957](https://gamedev.stackexchange.com/a/58412/74957)).
* Switch to sampler objects.
* Some kind of GUI.
* Atmospheric scattering: ([https://github.com/korgan00/TFG-Atmospheric-Scattering](https://github.com/korgan00/TFG-Atmospheric-Scattering)) or ([https://ebruneton.github.io/precomputed_atmospheric_scattering/](https://ebruneton.github.io/precomputed_atmospheric_scattering/))

## Pipe-dreaming
* CDLOD Terrain.
* Planetary rendering.
* Global illumination (Cascaded Light Propagation Volumes).

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
* _Variance Shadow Mapping_ ([http://developer.download.nvidia.com/SDK/10.5/direct3d/Source/VarianceShadowMapping/Doc/VarianceShadowMapping.pdf](http://developer.download.nvidia.com/SDK/10.5/direct3d/Source/VarianceShadowMapping/Doc/VarianceShadowMapping.pdf))
* _Tessellated Terrain Rendering with Dynamic LOD_ ([http://victorbush.com/2015/01/tessellated-terrain/](http://victorbush.com/2015/01/tessellated-terrain/))
