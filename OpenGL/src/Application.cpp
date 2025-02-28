#include <GL/glew.h>
#include <GLFW/glfw3.h>
// refer to docs.gl to understand any function and its purpose

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "tests/TestClearColor.h"
#include "tests/TestTriangle.h"
#include "tests/TestTexture2D.h"
#include "tests/TestPyramid.h"
#include "tests/TestUVSphere.h"
#include "tests/TestCubeRendering.h"
#include "tests/TestSquare.h"
#include "tests/TestCubeRenderingWTex.h"
#include "tests/TestFBMPlane.h"
#include "tests/TestHeightMap.h"
#include "tests/TestLoadModels.h"

#include "tests/TestParticleCPU.h"


#include "Shader_t.h"
#include "Camera.h"
#include "CameraController.h"
#include "Window.h"


// below code enforece the use of Nvidia GPU
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main(void)
{
    Window window("OpenGL Project", false);
    window.displayGPUDetails();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
    ImGui_ImplOpenGL3_Init();

    test::Test* currentTest = nullptr;
    test::TestMenu* menu = new test::TestMenu(currentTest);
    currentTest = menu;

    menu->AddTest<test::TestClearColor>("Clear Color");
    menu->AddTest<test::TestTriangle>("Render 2D Traingle");
    menu->AddTest<test::TestSquare>("Render 2D Square");
    menu->AddTest<test::TestTexture2D>("2D Texture");
    menu->AddTest<test::TestPyramid>("3D Textured Pyramid", window.GetWindow());
    menu->AddTest<test::TestUVSphere>("Render Sphere", window.GetWindow());
    menu->AddTest<test::TestCubeRendering>("Material Cube Rendering", window.GetWindow());
    menu->AddTest<test::TestCubeWithTex>("Cube With Texture", window.GetWindow());
    menu->AddTest<test::TestFBMPlane>("Fractal Brownian Motion Plane", &window);
    menu->AddTest<test::TestHeightMap>("Height Map", &window);
    menu->AddTest<test::TestLoadModels>("Load Models", &window);
    menu->AddTest<test::TestParticleCPU>("Particles CPU", &window);

    float lastFrameTime = 0.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        float currentFrameTime = float(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Check for backspace press and go back to menu
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        {
            if (currentTest && currentTest != menu)
            {
                glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                delete currentTest;
                currentTest = menu;
            }
        }

        // Clear the screen
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        if (currentTest)
        {
            currentTest->OnUpdate(deltaTime, window.GetWindow());
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != menu && ImGui::Button("<- or Backspace"))
            {
                glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                delete currentTest;
                currentTest = menu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        // IMGUI Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        window.Update();
    }

    if (currentTest != menu)
        delete menu;
    delete currentTest;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return 0;
}

// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;

//int main(void)
//{
//    Window window("Assimp Model loading");
//
//    glEnable(GL_DEPTH_TEST); // Enabling depth testing allows rear faces of 3D objects to be hidden behind front faces.
//    glEnable(GL_MULTISAMPLE); // Anti-aliasing
//    glEnable(GL_BLEND); // GL_BLEND for OpenGL transparency which is further set within the fragment shader. 
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    // Print GPU name and vendor
//    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
//    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
//
//    Shader shader("res/shaders/modelVS.glsl", "res/shaders/modelFS.glsl");
//
//    Camera camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
//	CameraController cameraController(window.GetWindow(), camera);
//
//    Model model_testing("res/objects/backpack/backpack.obj");
//    //Model model_testing("res/objects/spider/spider.obj");
//    //Model model_testing("res/objects/GoingMerry/GoingMerry.obj");
//    //Model model_testing("res/objects/conimbriga_complete_portugal.glb");
//    //Model model_testing("res/objects/model_testing.obj");
//    //Model model_testing("res/objects/ClassicFormula1/cf1car.obj");
//    //Model model_testing("res/objects/wb/WhiteBeard.obj"); 
//    //Model model_testing("res/objects/monkey/monkey.obj");
//    //Model model_testing("res/objects/porsche/Porsche_911_GT2.obj");
//    //Model model_testing("res/objects/luffy_hat.glb");
//    //Model model_testing("res/objects/ordinary_house/ordinary_house.obj");
//    //Model model_testing("res/objects/dabrovic-sponza/sponza.obj");
//    //Model model_testing("res/objects/P-51Mustang/P-51 Mustang.obj"); // not working
//    //Model model_testing("res/objects/knight/armor2021.obj");
//    
//    std::cout << "Model created" << std::endl;
//
//    shader.use();
//    shader.setVec3("lightPos", 5.0f, 5.0f, 5.0f);
//    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
//    shader.Unbind();
//
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    while (!glfwWindowShouldClose(window.GetWindow()))
//    {
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input camera
//        cameraController.Update(deltaTime);
//
//        // render
//        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        shader.use();
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), cameraController.GetAspectRatio(), 0.1f, 100.0f);
//        shader.setMat4("model", glm::mat4(1.0f));
//        shader.setMat4("view", camera.GetViewMatrix());
//        shader.setVec3("viewPos", camera.Position);
//        shader.setMat4("projection", projection);
//        for (unsigned int i = 0; i < model_testing.num_meshes; ++i)
//        {
//            shader.setVec3("diffuseColor", model_testing.mesh_list[i].diffuseColor.r, model_testing.mesh_list[i].diffuseColor.g, 
//                model_testing.mesh_list[i].diffuseColor.b);
//            shader.setBool("isTex", model_testing.mesh_list[i].hasTexture);
//            
//            if (model_testing.mesh_list[i].hasTexture)
//            {
//                glActiveTexture(GL_TEXTURE0);
//                glBindTexture(GL_TEXTURE_2D, model_testing.mesh_list[i].tex_handle); // Bind texture for the current mesh
//            }
//
//            glBindVertexArray(model_testing.mesh_list[i].VAO);
//            glDrawElements(GL_TRIANGLES, (GLsizei)model_testing.mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
//            glBindVertexArray(0);
//        }
//        shader.Unbind();
//        window.Update();
//    }
//
//    exit(EXIT_SUCCESS);
//}