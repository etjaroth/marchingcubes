#pragma once
#include "Config.h"
#include "Shader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"
#include "Fence.h"

class DistantLandscape
{
	int vertexCubeDimensions;
	glm::ivec2 pos;

	Heightmap& heightmapGenerator;
	SSBOComputeShader& genVerticies;

	enum class RenderingStages { start = 0, genHeightmap, genVerticies, done, size } current_step = RenderingStages::start; // size should always be last
	Fence fence;

	// rendering
	GLuint VAO = 0;
	GLuint EBO = 0;
	GLuint HEIGHTMAP = 0;
	const unsigned int SIZEOF_VERTEX = 4 * sizeof(float) * 3;
	std::vector<float> verticies;
	std::vector<unsigned int> indicies;

public:
	DistantLandscape(int vertexCubeDimensions, glm::ivec2 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies);
	~DistantLandscape();
	void renderCubes(Shader* shader);
};

