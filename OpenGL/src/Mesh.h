#pragma
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "IndexBuffer.h"
#include "Shader_t.h"

#define MAX_BONE_INFLUENCE 4

struct VertexData
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct TextureData
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh 
{
public:
    // mesh Data
    std::vector<VertexData>       vertices;
    std::vector<uint32_t>         indices;
    std::vector<TextureData>      textures;
    std::unique_ptr <VertexArray> VAO;

    // constructor
    Mesh(std::vector<VertexData> vertices, std::vector<unsigned int> indices, std::vector<TextureData> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    ~Mesh()
    {
		vertices.clear();
		indices.clear();
		textures.clear();
        VAO.reset();
		VBO.reset();
		EBO.reset();
    }

    // render the mesh
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
		VAO->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        VAO->Unbind();
    }

private:
    
    std::unique_ptr<VertexBuffer> VBO;
    std::unique_ptr<IndexBuffer> EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
		VAO = std::make_unique<VertexArray>();
		VBO = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(VertexData));   
        EBO = std::make_unique<IndexBuffer>(&indices[0], indices.size());

        VertexBufferLayout layout;
		layout.Push<float>(3);          // position
		layout.Push<float>(3);          // normal
		layout.Push<float>(2);          // texCoords
		layout.Push<float>(3);          // tangent
		layout.Push<float>(3);          // bitangent
		layout.Push<unsigned int>(4);   // ids
		layout.Push<float>(4);          // weights
        
		VAO->AddBuffer(*VBO, layout);
    }
};