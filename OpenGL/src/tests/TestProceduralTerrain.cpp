#include "TestProceduralTerrain.h"
#include "imgui/imgui.h"
#include <iostream>

#define STB_PERLIN_IMPLEMENTATION
#include "stb/stb_perlin.h"

namespace test
{
	TestProceduralTerrain::TestProceduralTerrain(Window* win)
		: m_Window(win),
		  m_planeColor{ 0.2f, 0.6f, 0.8f, 1.0f },
		  m_Camera(glm::vec3(0.0f, 13.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, -90.0f),
		  m_cameraController(m_Window->GetWindow(), m_Camera)
	{
		glEnable(GL_DEPTH_TEST);
		//glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_Window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
			{
				this->handleKeyPress(key, scancode, action, mods);
			});

		GeneratePlane();

		m_Shader = std::make_unique<Shader>("res/shaders/terrain/vertexShader.glsl", "res/shaders/terrain/fragmentShader.glsl");
		m_Renderer = std::make_unique<Renderer>();
	}

	TestProceduralTerrain::~TestProceduralTerrain()
	{
		m_plane.VAO.reset();
		m_plane.VBO.reset();
		m_plane.IBO.reset();
		m_Shader.reset();
		m_Renderer.reset();

		m_Window->setCustomKeyCallback(nullptr);
	}

	void TestProceduralTerrain::UpdatePlaneVertices()
	{
		if (m_PlaneWidth <= 0)
		{
			m_PlaneWidth = 1.0f;
		}
		if (m_PlaneHeight <= 0)
		{
			m_PlaneHeight = 1.0f;
		}

		float halfWidth = m_PlaneWidth / 2.0f;
		float halfHeight = m_PlaneHeight / 2.0f;

		// Update positions in the existing vertex data
		m_Vertices[0] = -halfWidth;  // Vertex 0 x
		m_Vertices[2] = -halfHeight; // Vertex 0 z

		m_Vertices[6] = halfWidth;   // Vertex 1 x
		m_Vertices[8] = -halfHeight; // Vertex 1 z

		m_Vertices[12] = halfWidth;  // Vertex 2 x
		m_Vertices[14] = halfHeight; // Vertex 2 z

		m_Vertices[18] = -halfWidth; // Vertex 3 x
		m_Vertices[20] = halfHeight; // Vertex 3 z

		// Update the vertex buffer
		if (m_plane.VBO) 
		{
			m_plane.VBO->UpdateData(m_Vertices.data(), static_cast<unsigned int>(m_Vertices.size() * sizeof(float)));
		}
	}

	void TestProceduralTerrain::GeneratePlane()
	{
		// Clear previous data
		m_Vertices.clear();
		m_Indices.clear();

		m_plane.VAO.reset();
		m_plane.VBO.reset();
		m_plane.IBO.reset();

		// Grid resolution - this is key for terrain detail
		// Higher values create more detailed terrain but impact performance
		int resolution = 100; // Number of vertices per side

		// Calculate the spacing between vertices
		float gridSpacingX = m_PlaneWidth / (resolution - 1);
		float gridSpacingZ = m_PlaneHeight / (resolution - 1);

		// Generate vertices in a grid pattern
		for (int z = 0; z < resolution; z++) {
			for (int x = 0; x < resolution; x++) {
				// Calculate position of this vertex in the grid
				float posX = x * gridSpacingX - (m_PlaneWidth / 2.0f);
				float posZ = z * gridSpacingZ - (m_PlaneHeight / 2.0f);

				// Add vertex position (XYZ)
				m_Vertices.push_back(posX);   // X position
				m_Vertices.push_back(0.0f);   // Y position (will be displaced in shader)
				m_Vertices.push_back(posZ);   // Z position

				// Add normal (will be recalculated in the shader based on height)
				m_Vertices.push_back(0.0f);   // normal X
				m_Vertices.push_back(1.0f);   // normal Y
				m_Vertices.push_back(0.0f);   // normal Z
			}
		}

		// Generate indices for triangles
		for (int z = 0; z < resolution - 1; z++) {
			for (int x = 0; x < resolution - 1; x++) {
				// Calculate indices for the quad's corners
				int topLeft = z * resolution + x;
				int topRight = topLeft + 1;
				int bottomLeft = (z + 1) * resolution + x;
				int bottomRight = bottomLeft + 1;

				// First triangle (counter-clockwise winding)
				m_Indices.push_back(topLeft);
				m_Indices.push_back(bottomLeft);
				m_Indices.push_back(bottomRight);

				// Second triangle (counter-clockwise winding)
				m_Indices.push_back(topLeft);
				m_Indices.push_back(bottomRight);
				m_Indices.push_back(topRight);
			}
		}

		// Set up buffers with the grid data
		m_plane.VAO = std::make_unique<VertexArray>();
		m_plane.VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), static_cast<unsigned int>(m_Vertices.size() * sizeof(float)));
		m_plane.IBO = std::make_unique<IndexBuffer>(m_Indices.data(), static_cast<unsigned int>(m_Indices.size()));

		VertexBufferLayout layout;
		layout.Push<float>(3);    // position (xyz)
		layout.Push<float>(3);    // normal (xyz)
		m_plane.VAO->AddBuffer(*m_plane.VBO, layout);

		// Log info about the mesh
		std::cout << "Generated terrain mesh with " << resolution * resolution << " vertices and "
			<< (resolution - 1) * (resolution - 1) * 2 << " triangles." << std::endl;
	}

	void TestProceduralTerrain::handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			// this flag is used to rotate the plane
			isRotating = !isRotating; 
		}
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			// this flag is used to render the plane in wireframe mode
			isWireFrame = !isWireFrame;
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			// this flag is used to handle the FBM plane transformation, ie to have a const or move with time
			isMoving = !isMoving;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			cusorEnable = !cusorEnable;
			if (cusorEnable)
			{
				glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
	}

	void TestProceduralTerrain::OnUpdate(Timestep deltaTime, GLFWwindow* window)
	{
		m_cameraController.Update(deltaTime);

		if (isMoving)
		{
			lastTtime = float(glfwGetTime());;

		}

		if (isRotating)
		{
			horizontalrotationAngle += rotationSpeed * deltaTime; // Rotate FBM tterrain clockwise
		}
	}

	void TestProceduralTerrain::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_Shader->Bind();
		
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100.0f);
		//glm::mat4 view = m_Camera.GetViewMatrix();
		glm::mat4 view = glm::lookAt(
			m_Camera.Position,				// Camera position (above the terrain)
			glm::vec3(0.0f, 0.0f, 0.0f),    // Looking at the origin
			glm::vec3(0.0f, 0.0f, -1.0f)    // Up direction
		);
		m_Shader->setVec2("u_resolution", { m_Window->GetWindowWidth(), m_Window->GetWindowHeight() });
		m_Shader->setMat4("view", view);
		m_Shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(horizontalrotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		m_Shader->setMat4("model", model);
		m_Shader->setFloat("scale", m_Scale);
		m_Shader->setInt("seed", m_Seed);
		m_Shader->setInt("octaves", m_Octaves);
		m_Shader->setFloat("persistence", m_Persistence);
		m_Shader->setFloat("lacunarity", m_Lacunarity);
		m_Shader->setVec2("offset", m_Offset);
		m_Shader->Unbind();
		if (isWireFrame)
		{
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		}
		else
		{
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}
		m_Renderer->Draw(*m_plane.VAO, *m_plane.IBO, *m_Shader);

	}

	void TestProceduralTerrain::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		// Add controls for plane dimensions
		bool dimensionsChanged = false;
		dimensionsChanged |= ImGui::InputFloat("Plane Width", &m_PlaneWidth, 0.0f, 0.0f, "%.1f");
		dimensionsChanged |= ImGui::InputFloat("Plane Height", &m_PlaneHeight, 0.0f, 0.0f, "%.1f");
		ImGui::SliderFloat("Scale", &m_Scale, 0.3f, 20.0f, "%.2f");
		if (ImGui::InputInt("Octaves", &m_Octaves))
		{
			if (m_Octaves < 0)
			{
				m_Octaves = 0;
			}
		}
		ImGui::SliderFloat("Persistence", &m_Persistence, 0.0f, 1.0f, "%.1f");
		if (ImGui::InputFloat("Lacunarity", &m_Lacunarity, 0.0f, 0.0f, "%.1f")) 
		{
			if (m_Lacunarity < 1.0f) 
			{
				m_Lacunarity = 1.0f;
			}
		}
		ImGui::SliderInt("Seed", &m_Seed, 0, 1000);
		ImGui::SliderFloat("Offset X", &m_Offset.x, 0.0f, 30.0f, "%.1f");
		ImGui::SliderFloat("Offset Y", &m_Offset.y, 0.0f, 30.0f, "%.1f");


		if (dimensionsChanged) 
		{
			UpdatePlaneVertices(); // Use the efficient update method
		}

		ImGui::Text("Press below keys to enable/disable:\n");
		ImGui::Text("`R` -> Plane Rotation\n");
		ImGui::Text("`T` -> Rendering Plane in Wireframe\n");
		//ImGui::Text("`M` -> Plane Movement\n");
		//ImGui::Text("Press `Q` to enable/disable Cursor (Mouse pointer)\n");
	}
}
