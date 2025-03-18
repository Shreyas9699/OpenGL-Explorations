# OpenGL Explorations

This repository is a learning project that explores the fundamentals of 3D rendering with OpenGL. It includes a collection of experiments and demonstrations, integrated with ImGui for easy exploration and modification of parameters.

## Gallery
Here are some project screenshots and a video:

### Video
<div style="display: flex; justify-content: center; margin-bottom: 20px;">
    <video width="60%" controls>
        <source src="Screenshots/TestFBM.mp4" type="video/mp4">
        Your browser does not support the video tag.
    </video>
</div>

### Screenshots
<div class="gallery">
    <div class="image-container" onclick="openImage('Screenshots/TestMaterialCubeWLight.png', 'Material Cube')">
        <img src="Screenshots/TestMaterialCubeWLight.png">
        <div class="overlay">Material Cube</div>
    </div>
    <div class="image-container" onclick="openImage('Screenshots/TestSquare.png', 'Test Square')">
        <img src="Screenshots/TestSquare.png">
        <div class="overlay">Test Square</div>
    </div>
    <div class="image-container" onclick="openImage('Screenshots/TestTexCubeWLight.png', 'Textured Cube')">
        <img src="Screenshots/TestTexCubeWLight.png">
        <div class="overlay">Textured Cube</div>
    </div>
    <div class="image-container" onclick="openImage('Screenshots/TestTexPryd.png', 'Textured Pyramid')">
        <img src="Screenshots/TestTexPryd.png">
        <div class="overlay">Textured Pyramid</div>
    </div>
    <div class="image-container" onclick="openImage('Screenshots/TestTriangle.png', 'Test Triangle')">
        <img src="Screenshots/TestTriangle.png">
        <div class="overlay">Test Triangle</div>
    </div>
    <div class="image-container" onclick="openImage('Screenshots/TestUVSphere.png', 'UV Sphere')">
        <img src="Screenshots/TestUVSphere.png">
        <div class="overlay">UV Sphere</div>
    </div>
</div>

<!-- Fullscreen Image Modal -->
<div id="fullscreen-modal" class="fullscreen-modal">
    <span class="close-btn" onclick="closeImage()">❌</span>
    <img id="fullscreen-img">
    <div id="fullscreen-caption"></div>
</div>

<style>
    .gallery {
        display: flex;
        flex-wrap: wrap;
        gap: 10px;
        justify-content: center;
    }

    .image-container {
        position: relative;
        width: 30%;
        cursor: pointer;
    }

    .image-container img {
        display: block;
        width: 100%;
        border-radius: 8px;
        transition: transform 0.3s ease;
    }

    .image-container:hover img {
        transform: scale(1.05);
    }

    .overlay {
        position: absolute;
        bottom: 0;
        left: 0;
        right: 0;
        background: rgba(0, 0, 0, 0.6);
        color: white;
        text-align: center;
        padding: 10px;
        font-size: 14px;
        opacity: 0;
        transition: opacity 0.3s ease;
        border-radius: 0 0 8px 8px;
    }

    .image-container:hover .overlay {
        opacity: 1;
    }

    /* Fullscreen Modal */
    .fullscreen-modal {
        display: none;
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: rgba(0, 0, 0, 0.9);
        justify-content: center;
        align-items: center;
        flex-direction: column;
        z-index: 1000;
    }

    .fullscreen-modal img {
        max-width: 90%;
        max-height: 80%;
        border-radius: 8px;
    }

    .fullscreen-modal .close-btn {
        position: absolute;
        top: 20px;
        right: 30px;
        font-size: 30px;
        color: white;
        cursor: pointer;
        transition: 0.3s;
    }

    .fullscreen-modal .close-btn:hover {
        color: red;
    }

    #fullscreen-caption {
        color: white;
        margin-top: 10px;
        font-size: 18px;
        text-align: center;
    }
</style>

<script>
    function openImage(src, caption) {
        document.getElementById('fullscreen-img').src = src;
        document.getElementById('fullscreen-caption').innerText = caption;
        document.getElementById('fullscreen-modal').style.display = 'flex';
    }

    function closeImage() {
        document.getElementById('fullscreen-modal').style.display = 'none';
    }
</script>

## Features
  - Render Game Window (Clear Color)
  - Render Triangle
  - Render Square
  - Load 2D Texture
  - Render Sphere (UV Sphere)
  - Cube Rendering With Lighting Effect and Materials
  - Cube With Texture and Lighting
  - Fractal Brownian Motion Plane With Lighting
  - Load and Tessellate Height Map
  - Load 3D model using Assimp
  - Particles in 3D (Implements in CPU)

## Getting Started
You can follow same instrunctions on `https://github.com/Shreyas9699/opengl-base-template` to set it up.
<br/>__OR__<br/>
You can follow the _The Cherno_ [_Instrunctions_](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)


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
