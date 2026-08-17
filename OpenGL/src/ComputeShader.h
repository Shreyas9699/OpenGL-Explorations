#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

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
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
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