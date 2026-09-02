#pragma once
#include <GLFW/glfw3.h>

#include "Timestep.h"

class Timer
{
public:
	Timestep Tick()
	{
		float now = float(glfwGetTime());
		float dt = now - m_last;
		m_last = now;
		return Timestep(dt);
	}

private:
	float m_last = float(glfwGetTime());
};