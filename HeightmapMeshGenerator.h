#pragma once
#include "AbstractThreadTask.h"
#include "Config.h"
#include "TerrainMesh.h"

struct HeightmapData {
	std::vector<TerrainMesh::Vertex> verticies;
	std::vector<unsigned int> indicies;
};

class HeightmapMeshGenerator
	: public AbstractThreadTaskTemplate<std::shared_ptr<HeightmapData>>
{
	int vertexCubeDimensions;
	float* pixels;
	glm::vec3 pos;

	glm::ivec2 getCoord(unsigned int idx, int width, int objSize);
	unsigned int getIndex(glm::ivec2 coord, int width, int objSize);
	glm::vec4 getArrayElement(glm::ivec2 coord, int width, float* pixels);

public:
	HeightmapMeshGenerator(int vertexCubeDimensions, float*& refPixels, glm::vec3 pos);
	~HeightmapMeshGenerator();

	void operator()() override;
};

