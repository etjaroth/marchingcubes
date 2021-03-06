#pragma once
#include "Config.h"
#include "MarchingCubes.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"

class ChunkManager
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::ivec3 chunk_position;
	unsigned int chunk_size;

	std::unordered_map<triple<int>, std::shared_ptr<MarchingCubes>, tripleHashFunction> chunk_map; // 3D vector of pointers to MarchingCubes
	int radius; // distance from orgin that chunks are generated

	GLuint VAO;
	Heightmap heightmap_generator;
	ComputeShader fill_generator;
	SSBOComputeShader gen_indicies;
	SSBOComputeShader gen_verticies;

	void update_chunks();

public:

	ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* heightmap_shader, const char* fill_shader);
	~ChunkManager();
	void set_pos(glm::vec3 pos);
	void set_direction(glm::vec3 dir);

	void render(Shader* shader);
};

