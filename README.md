# OpenGL Explorations

This repository is a learning project that explores the fundamentals of 3D rendering with OpenGL. It includes a collection of experiments and demonstrations, integrated with ImGui for easy exploration and modification of parameters.

## Gallery
Here are some project screenshots and a video:

### Screenshots
<div style="display: flex; flex-wrap: wrap; gap: 10px;">
  <img src="Screenshots/TestTriangle.png" width="30%">
  <img src="Screenshots/TestSquare.png" width="30%">
  <img src="Screenshots/TestTexPryd.png" width="30%">
  <img src="Screenshots/TestUVSphere.png" width="30%">
  <img src="Screenshots/TestMaterialCubeWLight.png" width="30%">
  <img src="Screenshots/TestTexCubeWLight.png" width="30%">
  <img src="Screenshots/TestParticleGPUPoint.png" width="30%">
</div>

### Videos
| [![Video 1](http://img.youtube.com/vi/pW238u3x3xw/0.jpg)](https://www.youtube.com/watch?v=pW238u3x3xw) | [![Video 2](http://img.youtube.com/vi/1TuheDbquEs/0.jpg)](https://www.youtube.com/watch?v=1TuheDbquEs) |
| --- | --- |
| [![Video 3](http://img.youtube.com/vi/TzpNCsVMecA/0.jpg)](https://www.youtube.com/watch?v=TzpNCsVMecA) |


## Features
  - Render Game Window (Clear Color)
  - Render Triangle
  - Render Square
  - Load 2D Texture
  - Render Sphere (UV Sphere)
  - Cube Rendering With Lighting Effect and Materials
    - Multiple material options
    - Phong lighting model 
  - Cube With Texture and Lighting
    - Textured Cube with texture Diffuse, Emission and Specular
    - Phong lighting model with Attenuation
  - Fractal Brownian Motion Plane With Lighting
    - Fractal Brownian Motion Plane with Wireframe view, Rotation and each point movement.
    - Phong lighting model
  - Load and Tessellate Height Map
    - Load Height Map from png
    - Uses Tessellation and interpolation to generate 3D Height map from png
    - Uses FOV to dynamically change the tessellation levels 
    - Renders Normals
    - Uses Frustum culling
  - Load 3D model using Assimp
    - Load 3D model from obj file
    - Render 3D model with Phong lighting model
    - Renders Normals
  - Particles in 3D (Implements in CPU)
    - Uses CPU to create, update and destroy particles from emitter
    - Max number of particle alive per frame is ~30K (with Avg of 22 FPS)
  - Particles in 3D (Implements in GPU Nvidia GTX 1050)
    - Uses CPU to create, update and destroy particles from emitter
    - Max number of particle alive per frame is ~1.5 million (with Avg of 120 FPS)

## Getting Started
You can follow same instructions on `https://github.com/Shreyas9699/opengl-base-template` to set it up.
<br/>__OR__<br/>
You can follow _The Cherno_ [_Instructions_](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)


### Prerequisites

- OpenGL 3.3 or higher
- GLFW
- GLEW
- GLM
- stb_image
- ImGui

## Usage

- Use the ImGui interface to modify rendering parameters in real-time.
- Explore different rendering techniques by selecting from the available demos.


## Acknowledgments

- [The Cherno](https://www.youtube.com/@TheCherno) for the OpenGL tutorials and guidance.
- [LearnOpenGL](https://learnopengl.com/) for providing excellent tutorials and resources.
- [The Book of shader](https://thebookofshaders.com/) to guide through the complex shader.