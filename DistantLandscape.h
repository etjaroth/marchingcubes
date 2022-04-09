#pragma once
#include "Config.h"
#include "Shader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"
#include "Fence.h"
#include "TerrainMesh.h"

class DistantLandscape
{
	int vertexCubeDimensions;
	glm::ivec3 pos;

	Heightmap& heightmapGenerator;
	SSBOComputeShader& genVerticies;

	enum class RenderingStages { genHeightmap=0, genVerticies, done, empty, size } currentStep = RenderingStages::genHeightmap; // size should always be last
	Fence fence;
	GLuint PBO = 0;
	float* pixels = nullptr;

	std::shared_ptr<TerrainMesh> mesh = nullptr;

	GLuint HEIGHTMAP = 0;
	const unsigned int SIZEOF_VERTEX = 4 * sizeof(float) * 3;
	struct Vertex;
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;

public:
	DistantLandscape(int _vertexCubeDimensions, glm::ivec3 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies);
	~DistantLandscape();

	void renderCubes(Shader* shader);

	void loadHeightmap();
	void generate();
};

