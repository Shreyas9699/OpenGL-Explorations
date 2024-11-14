#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	:m_Filepath(filepath), m_RendererID(0)
{
    ShaderProgramSource shaderSrc = ParseShader(filepath);;
    m_RendererID = CreateShader(shaderSrc.VertexSrc, shaderSrc.FragmentSrc);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::cout << "Shader file path: " << filepath << std::endl;
    std::ifstream fp(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(fp, line))
    {
        if (line.find("#SHADER") != std::string::npos)
        {
            if (line.find("VERTEX") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("FRAGMENT") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    //std::cout << "Vertex Shader \n" << ss[0].str() << std::endl;
    //std::cout << std::endl;
    //std::cout << "Fragment Shader \n" << ss[1].str() << std::endl;

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char* msg = (char*)malloc(len * sizeof(char));
        glGetShaderInfoLog(id, len, &len, msg);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shaders!" << std::endl;
        std::cout << msg << std::endl;
        free(msg);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int val)
{
    GLCall(glUniform1i(GetUniformLocation(name), val));
}

void Shader::SetUniform1f(const std::string& name, float val)
{
    GLCall(glUniform1f(GetUniformLocation(name), val));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4 matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name) const
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location =  glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: Uniform '" << name << "' doesn't exists!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}