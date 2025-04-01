#include "TestFireSimulation.h"
#include "imgui/imgui.h"

namespace test
{
	TestFireSimulation::TestFireSimulation(Window* window)
		: m_Window(window),
		m_Camera(glm::vec3(20.0f, 0.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), -140),
		m_cameraController(m_Window->GetWindow(), m_Camera)
	{
		m_Camera.IncreaseOutlier(50.0f);
		m_Window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
			{
				this->handleKeyPress(key, scancode, action, mods);
			});
		glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		m_Shader = std::make_unique<Shader>("res/shaders/fireParticle/fire_VS.glsl",
			"res/shaders/fireParticle/fire_FS.glsl");
		m_XZPlane = std::make_unique<XZPlaneGrid>(m_Window->GetAspectRatio(), m_GridSize, m_Near, m_Far);
		m_ParticleSys = std::make_unique<ParticleSystemGPU<FireParticle>>("fire");

		emitterProp.shape = EmitterShape::CIRCLE;
		emitterProp.emissionRate = 20000;
		m_ParticleSys->SetEmitter(emitterProp);
		m_ParticleSys->SetParticleSizeBegin(20.0f);
	}

	TestFireSimulation::~TestFireSimulation()
	{
		m_ParticleSys.reset();
		m_XZPlane.reset();
		m_Shader.reset();

		m_Window->setCustomKeyCallback(nullptr);
	}

	void TestFireSimulation::handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			isGridEnabled = !isGridEnabled;
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

	void TestFireSimulation::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), m_Near, m_Far);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_cameraController.Update(deltaTime);
		m_ParticleSys->Update(deltaTime, projection * view);

		m_Shader->Bind();
		m_Shader->setFloat("radius", emitterProp.radius);
		m_Shader->Unbind();

		m_ElapsedTime += deltaTime;

		if (m_GridSize != m_GridSizePrev)
		{
			m_GridSizePrev = m_GridSize;
			m_XZPlane.reset();
			m_XZPlane = std::make_unique<XZPlaneGrid>(m_Window->GetAspectRatio(), m_GridSize, m_Near, m_Far);
		}
	}

	void TestFireSimulation::OnRender()
	{
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), m_Near, m_Far);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_scalingFactor = m_Near * m_Window->GetWindowHeight() * glm::radians(m_Camera.Zoom);

		if (isGridEnabled)
		{
			m_XZPlane->Render(view, projection);
		}

		m_Shader->Bind();
		m_Shader->setMat4("projection", projection);
		m_Shader->setMat4("view", view);
		m_Shader->setFloat("scaling_factor", m_scalingFactor);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		m_Shader->setMat4("model", model);
		m_Shader->Unbind();

		m_ParticleSys->Render(*m_Shader);
	}

	void TestFireSimulation::OnImGuiRender()
	{
		ImVec4 enabledColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
		ImVec4 disabledColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		ImVec4 color;

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::Text("Number of particles alive: %d ", m_ParticleSys->GetActiveParticleCount());
		ImGui::Text("Elapsed Time: %.2f seconds", m_ElapsedTime);
		
		ImGui::Text("Press below keys to enable/disable:\n");
		color = cusorEnable ? enabledColor : disabledColor;
		ImGui::Text("`Q` -> Cursor (Mouse pointer) ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (cusorEnable) ? "ENABLED" : "DISABLED");

		color = isGridEnabled ? enabledColor : disabledColor;
		ImGui::Text("`G` -> Enable XZ Plane Grid ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (isGridEnabled) ? "ENABLED" : "DISABLED");
		ImGui::BeginDisabled(!isGridEnabled);

		ImGui::Text("Grid size (-1 is infinite)");
		ImGui::SliderInt(" ", &m_GridSize, -1, 150);
		ImGui::EndDisabled();

		//  Emitter properties
		if (ImGui::CollapsingHeader("Particle Emitter Properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Particle Emitter Shape CIRCLE");
			if (ImGui::SliderInt("Emission Rate", &emitterProp.emissionRate, 10000, 70000))
			{
				// Update the particle system with the new emitter properties
				m_ParticleSys->SetEmissionRate(emitterProp.emissionRate);
			};
			if (ImGui::SliderFloat3("Rotation (X, Y, Z degrees)", (float*)&emitterProp.rotation, 0.0f, 360.0f, "%.1f"))
			{
				m_ParticleSys->SetEmitterRotation(emitterProp.rotation);
			}
			if (ImGui::SliderFloat("Emitter Radius", (float*)&emitterProp.radius, 1.0f, 50.0f, "%.1f"))
			{
				m_ParticleSys->SetEmitterRadius(emitterProp.radius);
			}
		}

		// Particle properties
		static float sizeBegin = m_ParticleSys->GetParticleSizeBegin();
		static float sizeEnd = m_ParticleSys->GetParticleSizeEnd();
		static float lifespan = m_ParticleSys->GetParticleLifespan();

		if (ImGui::CollapsingHeader("Particle Properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Particle Begin Size", &sizeBegin, 0.1f, 30.0f, "%.1f"))
			{
				m_ParticleSys->SetParticleSizeBegin(sizeBegin);
			}

			if (ImGui::SliderFloat("Particle End Size", &sizeEnd, 0.1f, 10.0f, "%.1f"))
			{
				m_ParticleSys->SetParticleSizeEnd(sizeEnd);
			}

			if (ImGui::SliderFloat("Particle lifespan (in sec)", (float*)&lifespan, 1.0f, 13.0f, "%.1f"))
			{
				m_ParticleSys->SetParticleLifespan(lifespan);
			}
		}
		m_ParticleSys->GuiRender();
	}
}