#pragma once
#include "Config.h"
#include "MarchingCubes.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"

class ChunkManager
{
	glm::vec3 position;
	glm::ivec3 chunk_position;
	unsigned int chunk_size;

	std::unordered_map<triple<int>, std::unique_ptr<MarchingCubes>, tripleHashFunction> chunk_map; // 3D vector of pointers to MarchingCubes
	unsigned int radius; // distance from orgin that chunks are generated
	const char* shader_file;

	void update_chunks();

public:
	ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, unsigned int r, const char * landscape_generator);
	void set_pos(glm::vec3 pos);

	void render(Shader* shader);
};

