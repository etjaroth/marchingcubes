#pragma once
#include "Config.h"
#include "Shader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"
#include "Fence.h"

class DistantLandscape
{
	glm::ivec3 pos;

	Heightmap& heightmapGenerator;
	SSBOComputeShader& genVerticies;

	enum class RenderingStages { start = 0, genHeightmap, genVerticies, done, size } current_step = RenderingStages::start; // size should always be last
	Fence fence;

	// rendering
	GLuint VAO = 0;

	GLuint VERTEX_SSBO = 0;
	const int VERTEX_SSBO_BINDING = 0;
	unsigned int VERTEX_SSBO_SIZE;
	const unsigned int SIZEOF_VERTEX = 4 * sizeof(float) * 3;

	GLuint EBO = 0;
	const int EBO_BINDING = 1;
	unsigned int EBO_SIZE = 0;

	GLuint INDIRECT_SSBO; // Holds the information needed for indirect rendering
	const int INDIRECT_SSBO_BINDING = 2;

	GLuint HEIGHTMAP = 0;

public:
	DistantLandscape(glm::ivec3 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies);
	~DistantLandscape();
	void renderCubes(Shader* shader);
};

