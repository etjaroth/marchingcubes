#pragma once
#include "Config.h"
#include "Shader.h"
#include "Mesh.h"
class MinModel
{
public:
	MinModel(char* path) {
		loadModel(path);
	}
	void draw(Shader& shader, Material material);
private:
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};