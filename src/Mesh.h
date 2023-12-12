#pragma once
#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

constexpr int MAX_BONE_INFLUENCE = 4;

	// minimal requirements for a mesh in openGL
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bittangent;
		// bone indexes which will influence this vertex
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		// weights from each bone
		float m_Weights[MAX_BONE_INFLUENCE];
	};

	struct Texture
	{
		unsigned int id;
		// e.g. (exempli gratia) diffuse or specular texture
		std::string type;
		std::string path; // we store the path of the texture to compare with other textures
	};

class Mesh {
public:

	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}

	void Draw(Shader& shader)
	{
		unsigned int diffuseNr{ 1 };
		unsigned int specularNr{ 1 };
		unsigned int normalNr{ 1 };
		unsigned int heightNr{ 1 };
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			// retrieve texture number (the N in diffuse_texture_N)
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		// unbind
		glActiveTexture(GL_TEXTURE0);
		
		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	// render data
	unsigned int VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);											// Normal comes from the vertex struct
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// texture coords
		glEnableVertexAttribArray(2);													// TexCoords comes from the vertex struct
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		// The memory layout of a struct is sequential
		// Vertex vertex;
		// vertex.Position = glm::vec3(0.2f, 0.4f, 0.6f);
		// vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		// vertex.TexCoords = glm::vec2(1.0f, 0.0f);
		// = [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f];
		//   |---------------------------------->| TexCoords
		//				^offsetof value^ 
		//	offsetof(Vertex, TexCoords) -> returns the Bytes from the first variable of vertex(struct) to texcoords
		// sizeof(8 floats * 4 bytes each) = 32 bytes

		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bittangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bittangent));
		// ids, i think only for bittangent
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		// weights, i think only for bittangent
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
		
		// unbind VAO
		glBindVertexArray(0);
	}
};
