#pragma once
// #define STB_IMAGE_IMPLEMENTATION
#include "Mesh.h"

#include <assimp/scene.h>

#include <vector>

// unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
	// model data
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	Model(std::string const &path, bool gamma = false);
	void Draw(Shader &shader);

private:
	void loadModel(std::string path);

	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// helper function to load and initialize textures from material
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

	unsigned int textureFromFile(const char *path, const std::string &directory, bool gamma);
};
