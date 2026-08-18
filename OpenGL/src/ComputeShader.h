#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class ComputeShader
{
public: 
	unsigned int ID;

	ComputeShader(const char* path)
	{
		std::string compCode;
		std::ifstream compShaderFile;

		compShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			compShaderFile.open(path);
			std::stringstream compShaderStream;
			compShaderStream << compShaderFile.rdbuf();
			compShaderFile.close();
			compCode = compShaderStream.str();

		}
		catch (std::ifstream::failure& e)
		{
			std::cerr << "[" << path << "]" << "ERROR::COMPUTESHADER::FILE_NOT_SUCCESSFULLY_READ: "
				<< e.what() << std::endl;
		}

		const char* compShaderCode = compCode.c_str();

		unsigned int compute;

		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &compShaderCode, NULL);
		glCompileShader(compute);
		checkCompileErrors(compute, "COMPUTE", path);

		ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM", path);

		glDeleteShader(compute);
	}

	~ComputeShader() { glDeleteProgram(ID); }

	void use() { glUseProgram(ID); }

	void Bind() { glUseProgram(ID); }

	void Unbind() { glUseProgram(0); }

	void deleteProgram() const { glDeleteProgram(ID); }

	void dispatch(unsigned int x, unsigned int y, unsigned int z) const
	{
		glUseProgram(ID);
		glDispatchCompute(x, y, z);
	}

	static void memoryBarrier(GLbitfield barriers) { glMemoryBarrier(barriers); }

	// utility uniform functions
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(GetUniformLocation(name), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(GetUniformLocation(name), 1, &value[0]);
	}

	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(GetUniformLocation(name), 1, &value[0]);
	}

	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(GetUniformLocation(name), 1, &value[0]);
	}

	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

private:
	// mutable, allows modification even in const methods
	mutable std::unordered_map<std::string, GLuint> m_UniformLocationCache;

	GLuint GetUniformLocation(const std::string& name) const
	{
		auto it = m_UniformLocationCache.find(name);
		if (it != m_UniformLocationCache.end())
		{
			return it->second;
		}
		GLuint uniformLoc = glGetUniformLocation(ID, name.c_str());
		m_UniformLocationCache[name] = uniformLoc;
		return uniformLoc;
	}

	void checkCompileErrors(GLuint shader, std::string type, const char* path = nullptr)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cerr << "ERROR::COMPUTESHADER_COMPILATION_ERROR of type: " << type << "\n" 
					<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cerr << "[" << path << "]"
					<< "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
					<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};