#pragma once
#include "Config.h"
#include "DistantLandscape.h"
#include "MarchingCubes.h"

class Chunk
{
	unsigned int cutoffDistance;
	MarchingCubes* highDetail;
	DistantLandscape* lowDetail;

public:
	Chunk(unsigned int cutoffDistance, int cubeSize, glm::ivec3 position, Heightmap& heightmap_generator_ptr, ComputeShader& fill_generator_ptr, ComputeShader& lightingCalculatorPtr, SSBOComputeShader& gen_indices_ptr, SSBOComputeShader& gen_verticies_ptr, std::shared_ptr<ThreadManager> threadManager);
	~Chunk();
	Chunk(Chunk& other) = delete;
	Chunk(Chunk&& other) noexcept;
	Chunk& operator=(Chunk&& other) noexcept;
	Chunk& operator=(Chunk& other) = delete;

	void render(Shader* shader, double distance);
};