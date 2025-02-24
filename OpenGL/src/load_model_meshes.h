#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <string>

#include "Shader_t.h"

class Model
{
private:
	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	aiNode* root_node = nullptr;

	struct Mesh
	{
		unsigned int VAO, VBO1, VBO2, VBO3, EBO; 

		std::vector<glm::vec3> vert_positions;
		std::vector<glm::vec3> vert_normals;
		std::vector<glm::vec2> tex_coords;
		std::vector<unsigned int> vert_indices;
		unsigned int mesh_num;   
		unsigned int tex_handle;
		bool hasValidTexture = false;
	};

	struct TextureData
	{
		unsigned int tex_handle;
		unsigned int sampler_location;
		std::string image_name;
	};

	struct VertexData
	{
		glm::vec3 vert_position;
		glm::vec3 vert_normal;
		glm::vec2 tex_coords;
		unsigned int mesh_num;
		unsigned int sampler_array_pos;
	};

	struct Mesh_1BO
	{
		unsigned int VAO, VBO, EBO;
		std::vector<unsigned int> vert_indices;
		std::vector<VertexData> vertex_data;
	};

	int skip;
	double time;
	double prev_time;
	unsigned int draw_method;
	unsigned int num_meshes;
	std::vector<Mesh> mesh_list;
	std::vector<TextureData> texture_list;
	Mesh_1BO meshesVBOCombined;
	std::string directory;

	Shader& shader;
	unsigned int draw_multiple_meshes_sampler_pos0_val;
	unsigned int rendering_multiple_meshes_loc; // Uniform required because multiple meshes (draw method 0) doesn't pass mesh number via shader input attribute.
	unsigned int meshes_combined_loc;

public:
	Model(const char* model_path, Shader& main_shader, unsigned int draw_method = 0)
		: shader(main_shader),
		  draw_method(draw_method)
	{
		skip = 0;
		time = 0;
		prev_time = 0;

		// http://assimp.sourceforge.net/lib_html/postprocess_8h.html (See: aiPostProcessSteps) (Flag options)
		// scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		// scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);
		// scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);
		// scene = importer.ReadFile(model_path, NULL);	

		std::string model_path_str(model_path);
		directory = model_path_str.substr(0, model_path_str.find_last_of('/'));
		std::cout << "Model : " << model_path_str << std::endl;

		load_model(); // Uncomment only one of these two load model functions.
		// load_model_cout_console();

		configure_draw_calls();
	}

	~Model() 
	{
		// Cleanup individual meshes
		for (auto& mesh : mesh_list) 
		{
			glDeleteVertexArrays(1, &mesh.VAO);
			glDeleteBuffers(1, &mesh.VBO1);
			glDeleteBuffers(1, &mesh.VBO2);
			glDeleteBuffers(1, &mesh.VBO3);
			glDeleteBuffers(1, &mesh.EBO);
		}

		// Cleanup combined mesh data
		glDeleteVertexArrays(1, &meshesVBOCombined.VAO);
		glDeleteBuffers(1, &meshesVBOCombined.VBO);
		glDeleteBuffers(1, &meshesVBOCombined.EBO);

		// Cleanup textures
		for (const auto& texture : texture_list) 
		{
			glDeleteTextures(1, &texture.tex_handle);
		}

		// Clear all vectors
		mesh_list.clear();
		texture_list.clear();
		meshesVBOCombined.vertex_data.clear();
		meshesVBOCombined.vert_indices.clear();
	}

	void drawMesh()
	{
		if (draw_method == 0)
			draw_multiple_meshes();
		if (draw_method == 1)
			drawSingleCombinedMesh();
	}

private:
	void load_model()
	{
		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "ERROR::ASSIMP::Importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
		else
		{
			num_meshes = scene->mNumMeshes;
			mesh_list.resize(num_meshes);

			std::cout << "   ASSIMP....Total Number of Mesh: " << num_meshes << std::endl;

			aiMesh* mesh{};
			unsigned int total_num_vertices = 0;
			unsigned int total_num_indices = 0;

			// (1) Loop through all the model's meshes
			// -----------------------------------------------------
			for (unsigned int i = 0; i < num_meshes; ++i)
			{
				mesh_list[i].mesh_num = i; // Fed in to vertex shader as input attribute (Used to identify and transform meshes independently of one another)
				mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html				
				mesh_list[i].hasValidTexture = false;

				total_num_vertices += mesh->mNumVertices;
				//std::cout << "   Mesh: " << i << " --- mesh[i].mNumVertices: " << mesh->mNumVertices << "\n";

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html			

				// This loop will only run once (i.e. there's only 1 texture per mesh)
				for (unsigned int tex_count = 0; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // Also, only using: aiTextureType_DIFFUSE.
				{
					mesh_list[i].hasValidTexture = true; // if not texture then it wont enter loop
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string);	 // Acquire the name of the image file to be loaded.			

					// (2) Load mesh [i]'s texture if not already loaded
					// ---------------------------------------------------------------
					int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.

					if (already_loaded == -1) // Image not yet loaded so now attempt to load it.
					{
						bool load_success = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_success);

						if (load_success) // Although do nothing if the image fails to load.
						{
							TextureData texture;
							texture.image_name = string.C_Str();
							texture.tex_handle = texture_handle;

							texture_list.push_back(texture);
							mesh_list[i].tex_handle = texture_handle;
						}
						//std::cout << "\n";
					}
					else
						mesh_list[i].tex_handle = already_loaded; // Assign existing texture handle.
				}
				// (3) Loop through all mesh [i]'s vertices
				// ---------------------------------------------------
				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);

					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

					if (mesh->HasTextureCoords(0)) // Only slot [0] is in question.
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						mesh_list[i].tex_coords.push_back(tex_coords);
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
				}
				// (4) Loop through all mesh [i]'s Indices
				// --------------------------------------------------
				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				{
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
						mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
					total_num_indices += mesh->mFaces[i3].mNumIndices;
				}
				set_buffer_data(i); // Set up: VAO, VBO and EBO.
			}
			std::cout << "   ------------------------------------------\n";
			std::cout << "   ASSIMP  total_num_vertices: " << total_num_vertices << "\n";
			std::cout << "   ASSIMP  total_num_indices: " << total_num_indices << "\n\n";
		}
	}

	void load_model_cout_console()
	{
		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "ERROR::ASSIMP::Importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
		else
		{
			num_meshes = scene->mNumMeshes;
			mesh_list.resize(num_meshes);

			std::cout << "\n\n   Start of Assimp Loading Meshes & Analysis";
			std::cout << "\n   -----------------------------------------";

			root_node = scene->mRootNode;

			std::cout << "\n   node->mNumMeshes: " << root_node->mNumMeshes;
			std::cout << "\n   node->mName.C_Str(): " << root_node->mName.C_Str();
			std::cout << "\n\n   node->mNumChildren: " << root_node->mNumChildren;
			// ------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < root_node->mNumChildren; ++i)
			{
				std::cout << "\n   node->mChildren[i]->mName.C_Str(): " << root_node->mChildren[i]->mName.C_Str();
				std::cout << "\n   node->mChildren[i]->mNumMeshes: " << root_node->mChildren[i]->mNumMeshes;
			}
			std::cout << "\n\n   scene->HasMaterials(): " << scene->HasMaterials();
			// ------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
				std::cout << "\n   scene->mMaterials[i]->GetName(): " << scene->mMaterials[i]->GetName().C_Str();

			std::cout << "\n\n   scene->HasTextures(): " << scene->HasTextures();

			aiMesh* mesh{};

			int total_num_indices = 0;

			// (1) Loop through all the model's meshes
			// -----------------------------------------------------
			std::cout << "\n   scene->mNumMeshes: " << num_meshes;
			std::cout << "\n   ********************\n";
			// ---------------------------------------------------------
			for (unsigned int i = 0; i < num_meshes; ++i) // In this case... scene->mNumMeshes = node->mChildren[i]->mNumMeshes
			{
				mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html

				std::cout << "\n\n   mesh->mMaterialIndex: " << mesh->mMaterialIndex;
				std::cout << "\n   ----------------------- ";
				std::cout << "\n   mesh->mName.C_Str(): " << mesh->mName.C_Str();

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html

				std::cout << "\n\n   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << material->GetTextureCount(aiTextureType_DIFFUSE);
				std::cout << "\n   material->GetTexture(aiTextureType_SPECULAR, tex_count, &string): " << material->GetTextureCount(aiTextureType_SPECULAR);
				std::cout << "\n   material->GetTexture(aiTextureType_AMBIENT, tex_count, &string): " << material->GetTextureCount(aiTextureType_AMBIENT) << "\n\n";
				mesh_list[i].hasValidTexture = false;

				unsigned int tex_count = 0;
				for (; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // The above std::cout reveals that only using: aiTextureType_DIFFUSE
				{
					mesh_list[i].hasValidTexture = true;
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string); // Acquire the name of the image file to be loaded.
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << string.C_Str() << "\n\n";

					// (2) Load mesh [i]'s texture if not already loaded
					// ---------------------------------------------------------------
					int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.
					std::cout << "   Loading Image\n";

					if (already_loaded == -1) // Image not yet loaded.
					{
						bool load_complete = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_complete);

						if (load_complete) // Although do nothing if the image fails to load.
						{
							TextureData texture;
							texture.image_name = string.C_Str();
							texture.tex_handle = texture_handle;

							texture_list.push_back(texture);
							mesh_list[i].tex_handle = texture_handle;
						}
					}
					else // Assign existing texture handle.
					{
						std::string edited = string.C_Str();
						std::size_t position = edited.find_last_of("\\");

						std::cout << "   Image file: " << edited.substr(position + 1) << " (is already loaded)";
						mesh_list[i].tex_handle = already_loaded;
					}
				}
				if (tex_count == 0)
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): No image has been applied to this mesh\n\n";
				else
					std::cout << "\n";

				for (unsigned int slot = 0; slot < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++slot)
					std::cout << "   mesh->HasTextureCoords(" << slot << "): " << mesh->HasTextureCoords(slot) << "\n";

				std::cout << "\n   Mesh index: " << i << " (mesh->mNumVertices: " << mesh->mNumVertices << ")";
				std::cout << "\n   ------------------------------------- ";

				// (3) Loop through all mesh [i]'s vertices
				// ---------------------------------------------------
				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);

					std::cout << "\n   Count: " << i2;
					std::cout << "\n   mesh->mVertices[" << i2 << "].x: " << position.x;
					std::cout << "\n   mesh->mVertices[" << i2 << "].y: " << position.y;
					std::cout << "\n   mesh->mVertices[" << i2 << "].z: " << position.z;

					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
						std::cout << "\n   mesh->mNormals[" << i2 << "] X: " << normal.x << " Y: " << normal.y << " Z: " << normal.z;
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

					if (mesh->HasTextureCoords(0)) // Above for loop: AI_MAX_NUMBER_OF_TEXTURECOORDS reveals that only slot [0] is in question.
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						mesh_list[i].tex_coords.push_back(tex_coords);
						std::cout << "\n   mesh->mTextureCoords[0][" << i2 << "] X: " << tex_coords.x << " Y: " << tex_coords.y;
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
				}
				std::cout << "\n\n   mesh->mNumFaces: " << mesh->mNumFaces << "\n";
				std::cout << "   ------------------ ";

				// (4) Loop through all mesh [i]'s Indices
				// --------------------------------------------------
				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				{
					std::cout << "\n";
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
					{
						std::cout << "   mesh->mFaces[" << i3 << "].mIndices[" << i4 << "]: " << mesh->mFaces[i3].mIndices[i4] << "\n";
						mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
						++total_num_indices;
					}
				}
				std::cout << "\n   Total number of indices: " << total_num_indices;
				std::cout << "\n   **************************\n";
				total_num_indices = 0;

				set_buffer_data(i); // Set up: VAO, VBO and EBO.
			}
			// Look to see if each mesh's texture handle corresponds correctly to the loaded image
			// ----------------------------------------------------------------------------------------------------------------
			std::cout << "\n   Look to see if each mesh's texture handle corresponds correctly to the loaded image";
			std::cout << "\n   -----------------------------------------------------------------------------------\n";
			if (texture_list.size() > 0)
				for (unsigned int i = 0; i < texture_list.size(); ++i)
				{
					std::cout << "   image_list[" << i << "].imageID: " << texture_list[i].tex_handle << "... image_list[" << i << "].image_name: " << texture_list[i].image_name << "\n";

					for (unsigned int i2 = 0; i2 < num_meshes; ++i2)
						if (texture_list[i].tex_handle == mesh_list[i2].tex_handle)
							std::cout << "   mesh_list[" << i2 << "].tex_handle: " << mesh_list[i2].tex_handle << "\n";
					std::cout << "\n";
				}
			else
				std::cout << "   ***** No images have been loaded\n";
		}
	}

	void configure_draw_calls()
	{
		rendering_multiple_meshes_loc = glGetUniformLocation(shader.ID, "rendering_multiple_meshes");
		meshes_combined_loc = glGetUniformLocation(shader.ID, "meshes_combined");


		if (draw_method == 1)
		{
			combine_meshes_1VBO();
			populate_sampler_array_setup(texture_list);
		}
		else
		{
			if(draw_method != 0)
				std::cout <<"    MODEL::Invalid Draw method, default = 0" << std::endl;
			draw_multiple_meshes_sampler_pos0_val = glGetUniformLocation(shader.ID, "images[0]");
		}

	}

	void populate_sampler_array_setup(std::vector<TextureData>& texture_list)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			std::string index_num = std::to_string(i);
			std::string sampler_name = "images[" + index_num + "]";

			// Sampler location set below is received here via texture list (by reference)
			// ------------------------------------------------------------------------------------------------
			texture_list[i].sampler_location = glGetUniformLocation(shader.ID, sampler_name.c_str());
		}
	}

	void combine_meshes_1VBO()
	{
		std::cout << "\n   Analysing combined mesh data\n";
		std::cout << "   ----------------------------";

		VertexData data{};
		unsigned int offset = 0;

		for (unsigned int i = 0; i < num_meshes; i++)
		{
			for (unsigned int i2 = 0; i2 < mesh_list[i].vert_positions.size(); i2++) // Add attribute values for every vertex entry.
			{
				data.mesh_num = mesh_list[i].mesh_num; // Use for identifying and transforming meshes independently.

				data.vert_position = mesh_list[i].vert_positions[i2];
				data.vert_normal = mesh_list[i].vert_normals[i2];
				data.tex_coords = mesh_list[i].tex_coords[i2];

				if (mesh_list[i].hasValidTexture) 
				{
					for (unsigned int i3 = 0; i3 < texture_list.size(); i3++)
					{
						if (mesh_list[i].tex_handle == texture_list[i3].tex_handle) // Compare mesh handle to texture list handle.
						{
							data.sampler_array_pos = i3;
							break;
						}
					}
				}
				else 
				{
					data.sampler_array_pos = UINT_MAX;  // Indicate no valid texture
				}
				meshesVBOCombined.vertex_data.push_back(data); // Add vertex to meshes combined. 	
			}
			//std::cout << "\n   offset += (unsigned int)mesh_list[i].vert_positions.size(): " << offset << "\n";
			for (unsigned int i2 = 0; i2 < mesh_list[i].vert_indices.size(); i2++) // Offset by the total number of vertices in previous meshes.
			{
				meshesVBOCombined.vert_indices.push_back(mesh_list[i].vert_indices[i2] + offset);
				// std::cout << "   meshesVBOCombined.vert_indices.push_back(mesh_list[i].vert_indices[i2] + offset): " << mesh_list[i].vert_indices[i2] + offset << "\n";
			}
			offset += (unsigned int)mesh_list[i].vert_positions.size(); // Unsigned 32 bit is still over 2 billion.
		}
		set_buffer_data_combined_1VBO(); // Set up: VAO, VBO and EBO.

		std::cout << "\n   Meshes combined into struct lists\n";
		std::cout << "   ---------------------------------\n";
		std::cout << "   meshes_1VBO_combined.vertex_data.size(): " << meshesVBOCombined.vertex_data.size() << "\n\n";
	}

	void set_buffer_data(unsigned int index)
	{
		glGenVertexArrays(1, &mesh_list[index].VAO);
		glGenBuffers(1, &mesh_list[index].VBO1);
		glGenBuffers(1, &mesh_list[index].VBO2);
		glGenBuffers(1, &mesh_list[index].VBO3);
		glGenBuffers(1, &mesh_list[index].EBO);

		glBindVertexArray(mesh_list[index].VAO);

		// Vertex Positions
		// ---------------------
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_positions.size(), &mesh_list[index].vert_positions[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Vertex Normals
		// --------------------
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_normals.size(), &mesh_list[index].vert_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Texture Coordinates
		// ---------------------------
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh_list[index].tex_coords.size(), &mesh_list[index].tex_coords[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		// Indices for: glDrawElements()
		// ---------------------------------------
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0); // Unbind VAO
	}

	void set_buffer_data_combined_1VBO()
	{
		glGenVertexArrays(1, &meshesVBOCombined.VAO);
		glGenBuffers(1, &meshesVBOCombined.VBO);
		glGenBuffers(1, &meshesVBOCombined.EBO);

		glBindVertexArray(meshesVBOCombined.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, meshesVBOCombined.VBO);
		glBufferData(GL_ARRAY_BUFFER, meshesVBOCombined.vertex_data.size() * sizeof(VertexData), &meshesVBOCombined.vertex_data[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, vert_normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tex_coords));

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(VertexData), (void*)offsetof(VertexData, mesh_num)); // Notice the "I"

		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(VertexData), (void*)offsetof(VertexData, sampler_array_pos)); // Notice the "I"

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshesVBOCombined.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshesVBOCombined.vert_indices.size(), &meshesVBOCombined.vert_indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);	
	}

	int is_image_loaded(std::string file_name)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			if (file_name.compare(texture_list[i].image_name) == 0)
			{
				return texture_list[i].tex_handle;
			}
		}
		return -1;
	}

	unsigned int load_texture_image(std::string file_name, bool& load_complete)
	{
		// stbi_set_flip_vertically_on_load(1); // Call this function if the image is upside-down.		

		std::size_t position = file_name.find_last_of("\\");
		file_name = file_name.substr(position + 1);
		std::string full_path = directory + '/' + file_name;

		int width, height, num_components;
		unsigned char* image_data = stbi_load(full_path.c_str(), &width, &height, &num_components, 0);

		unsigned int tex_handle;
		glGenTextures(1, &tex_handle);

		if (image_data)
		{
			GLenum format{};

			if (num_components == 1)
				format = GL_RED;
			else if (num_components == 3)
				format = GL_RGB;
			else if (num_components == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, tex_handle);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Recommended by NVIDIA Rep: https://devtalk.nvidia.com/default/topic/875205/opengl/how-does-gl_unpack_alignment-work-/

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
			// ----------------------------------------------------------------------------------------------------------------
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // GL_REPEAT... GL_MIRRORED_REPEAT... GL_CLAMP_TO_EDGE... GL_CLAMP_TO_BORDER.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

			// float border_colour[] = {0.45, 0.55, 0.95};
			// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour); // For above when using: GL_CLAMP_TO_BORDER		

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST... GL_LINEAR... GL_NEAREST_MIPMAP_NEAREST (See above link for full list)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR.

			load_complete = true;
			stbi_image_free(image_data);
			std::cout << "   Image loaded OK: " << file_name << "\n";
		}
		else
		{
			load_complete = false;
			stbi_image_free(image_data);
			std::cerr << "   STB_IMAGE::stbi_load::failed to load Image: " << file_name << "\n";
		}
		return tex_handle;
	}

	void draw_multiple_meshes()
	{
		glUniform1i(meshes_combined_loc, 0);
		glUniform1i(draw_multiple_meshes_sampler_pos0_val, 0); // Make sure sampler array is at position 0: image[0] used in fragment shader, is set to 0.
		glActiveTexture(GL_TEXTURE0);
		for (unsigned int i = 0; i < num_meshes; i++)
		{
			glUniform1i(rendering_multiple_meshes_loc, i);
			glBindTexture(GL_TEXTURE_2D, mesh_list[i].tex_handle); // Bind texture for the current mesh.	
			glBindVertexArray(mesh_list[i].VAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void drawSingleCombinedMesh()
	{
		glUniform1i(meshes_combined_loc, 1); // If-statement in fragment shader.
		glUniform1i(rendering_multiple_meshes_loc, -1);
		populate_sampler_array(texture_list);
		glBindVertexArray(meshesVBOCombined.VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)meshesVBOCombined.vert_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void populate_sampler_array(std::vector<TextureData> texture_list)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texture_list[i].tex_handle);
			glUniform1i(texture_list[i].sampler_location, i);
		}
	}
};