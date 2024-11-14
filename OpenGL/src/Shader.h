#pragma once

#include <string>
#include <unordered_map>

#include "glm/gtc/matrix_transform.hpp"

struct ShaderProgramSource
{
	std::string VertexSrc;
	std::string FragmentSrc;
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_Filepath;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;

	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name) const;

public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Set Uniforms
	void SetUniform1i(const std::string& name, int val);
	void SetUniform1f(const std::string& name, float val);
	void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3);
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
};