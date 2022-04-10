#pragma once
#include "Config.h"
#include "Chunk.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"
#include "ThreadManager.h"

class ChunkManager
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::ivec3 chunk_position;
	unsigned int chunk_size;
	glm::mat4 model;
	glm::mat4 invModel;

	std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> chunks; // 3D vector of pointers to MarchingCubes
	int radius; // distance from orgin that chunks are generated

	GLuint VAO;
	Heightmap heightmap_generator;
	ComputeShader fill_generator;
	ComputeShader lightingCalculator;
	SSBOComputeShader gen_indicies;
	SSBOComputeShader gen_verticies;

	std::shared_ptr<ThreadManager> threadManager = std::make_shared<ThreadManager>();
	//std::shared_ptr<ThreadManager> threadManager = nullptr;

	void update_chunks();

public:

	ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* heightmap_shader, const char* fill_shader, float scale=2.0f);
	~ChunkManager();
	void set_pos(glm::vec3 pos);
	void set_direction(glm::vec3 dir);

	void render(Shader* shader, double time, double dayNightSpeed);
};

