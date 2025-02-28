#include "TestParticleCPU.h"
#include "imgui/imgui.h"

test::TestParticleCPU::TestParticleCPU(Window* window)
	: m_Window(window),
	  m_Camera(glm::vec3(20.0f, 0.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), -140),
	  m_cameraController(m_Window->GetWindow(), m_Camera)
{
	m_Window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
		{
			this->handleKeyPress(key, scancode, action, mods);
		});
	glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	m_Shader = std::make_unique<Shader>("res/shaders/ParticleSysCPU/vertexShader.glsl", "res/shaders/ParticleSysCPU/fragmentShader.glsl");
	m_XZPlane = std::make_unique<XZPlaneGrid>(m_Window->GetAspectRatio(), m_GridSize, m_Near, m_Far);
	m_ParticleSys = std::make_unique <ParticleSystem>();

	emitterShape = { "POINT", "SPHERE", "CONE", "BOX" };

	// Setup a point emitter at position(0, 0, 0)
	emitterProp.shape = EmitterShape::POINT;
	m_ParticleSys->SetEmitter(emitterProp);

	// setup a sphere emitter
	/*emitterProp.shape = EmitterShape::SPHERE;
	m_ParticleSys->SetEmitter(emitterProp);*/
	

	// setup a cone emitter pointing downward
	/*emitterProp.shape = EmitterShape::CONE;
	emitterProp.radius = 5.0f;
	m_ParticleSys->SetEmitter(emitterProp);*/
	

   // setup a cone emitter pointing downward
	/*emitterProp.shape = EmitterShape::BOX;
	m_ParticleSys->SetEmitter(emitterProp);*/
}

test::TestParticleCPU::~TestParticleCPU()
{
	m_ParticleSys.reset();
	m_XZPlane.reset();
	m_Shader.reset();

	m_Window->setCustomKeyCallback(nullptr);
}

void test::TestParticleCPU::handleKeyPress(int key, int scancode, int action, int mods)
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

void test::TestParticleCPU::OnUpdate(Timestep deltaTime, GLFWwindow* win)
{
	m_cameraController.Update(deltaTime);
	m_ParticleSys->Update(deltaTime);

	if (m_GridSize != m_GridSizePrev)
	{
		m_GridSizePrev = m_GridSize;
		m_XZPlane.reset();
		m_XZPlane = std::make_unique<XZPlaneGrid>(m_Window->GetAspectRatio(), m_GridSize, m_Near, m_Far);
	}

	/*m_ParticleSys->SetEmitterPosition(m_EmitterPosition);
	m_ParticleSys->SetEmissionRate(m_EmissionRate);

	if (m_EmitterShapePrev != m_EmitterShape)
	{
		m_EmitterShapePrev = m_EmitterShape;
		m_ParticleSys.reset();
		m_ParticleSys = std::make_unique <ParticleSystem>();

		m_ParticleSys->SetEmitterShape(m_EmitterShape);
		

		if (m_EmitterShape == EmitterShape::POINT)
		{
			
		}
	}*/
}

void test::TestParticleCPU::OnRender()
{
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), m_Near, m_Far);
	glm::mat4 view = m_Camera.GetViewMatrix();

	if (isGridEnabled)
	{
		m_XZPlane->Render(view, projection);
	}

	m_Shader->Bind();
	m_Shader->setMat4("projection", projection);
	m_Shader->setMat4("view", view);

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	m_Shader->setMat4("model", model);
	m_Shader->Unbind();

	m_ParticleSys->Render(*m_Shader);
}

void test::TestParticleCPU::OnImGuiRender()
{
	ImVec4 enabledColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
	ImVec4 disabledColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
	ImVec4 color;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
	ImGui::Text("Number of particles alive: %d ", m_ParticleSys->GetNumOfParticles());

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

	//ImGui::Separator();
	//  Emitter properties
	if (ImGui::CollapsingHeader("Particle Emitter Properties", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Particle Emitter properties\n");
		if (ImGui::Combo("Particle Emitter Shape", &currentEmitterShapeIdx, emitterShape.data(), emitterShape.size()))
		{
			if (emitterShape[currentEmitterShapeIdx] == "POINT")
			{
				emitterProp.shape = EmitterShape::POINT;
			}
			else if (emitterShape[currentEmitterShapeIdx] == "SPHERE")
			{
				emitterProp.shape = EmitterShape::SPHERE;
			}
			else if (emitterShape[currentEmitterShapeIdx] == "CONE")
			{
				emitterProp.shape = EmitterShape::CONE;
			}
			else if (emitterShape[currentEmitterShapeIdx] == "BOX")
			{
				emitterProp.shape = EmitterShape::BOX;
			}
			m_ParticleSys->SetEmitter(emitterProp);
		}

		ImGui::SliderFloat("Particle Emitter EmissionRate", (float*)&emitterProp.emissionRate, 10.0f, 1500.0f);
		ImGui::BeginDisabled(emitterShape[currentEmitterShapeIdx] != "SPHERE" && emitterShape[currentEmitterShapeIdx] != "CONE");
		if (ImGui::SliderFloat("Particle Emitter Radius", (float*)&emitterProp.radius, 1.0f, 10.0f))
		{
			m_ParticleSys->SetEmitterRadius(emitterProp.radius);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(emitterShape[currentEmitterShapeIdx] != "CONE");
		if (ImGui::SliderFloat("Particle Emitter Angle", (float*)&emitterProp.angle, 0.0f, 360.0f))
		{
			m_ParticleSys->SetEmitterAngle(emitterProp.angle);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(emitterShape[currentEmitterShapeIdx] != "BOX");
		if (ImGui::SliderFloat3("Particle Emitter Dimensions", (float*)&emitterProp.dimensions, 1.0f, 10.0f))
		{
			m_ParticleSys->SetEmitterDimensions(emitterProp.dimensions);
		}
		ImGui::EndDisabled();
	}
	
	// Particle properties
	static glm::vec4 colorBegin = m_ParticleSys->GetarticleColorBegin();
	static glm::vec4 colorEnd = m_ParticleSys->GetarticleColorEnd();
	static float sizeBegin = m_ParticleSys->GetarticleSizeBegin();
	static float sizeEnd = m_ParticleSys->GetarticleSizeEnd();
	static float lifespan = m_ParticleSys->GetarticleLifespan();

	if (ImGui::CollapsingHeader("Particle Properties", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::ColorEdit4("Particle Begin Color", (float*)&colorBegin))
		{
			m_ParticleSys->SetParticleColorBegin(colorBegin);
		}

		if (ImGui::ColorEdit4("Particle End Color", (float*)&colorEnd))
		{
			m_ParticleSys->SetParticleColorEnd(colorEnd);
		}

		if (ImGui::SliderFloat("Particle Begin Size", (float*)&sizeBegin, 0.1f, 10.0f))
		{
			m_ParticleSys->SetParticleSizeBegin(sizeBegin);
		}

		if (ImGui::SliderFloat("Particle End Size", (float*)&sizeEnd, 0.1f, 10.0f))
		{
			m_ParticleSys->SetParticleSizeEnd(sizeEnd);
		}

		if (ImGui::SliderFloat("Particle lifespan (in sec)", (float*)&lifespan, 0.01f, 20.0f))
		{
			m_ParticleSys->SetParticleLifespan(lifespan);
		}
	}
}
