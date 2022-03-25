#pragma once
#include "Config.h"
#include "ComputeShader.h"
struct HeightmapTile;


class Heightmap {
	std::unordered_map<glm::ivec3, HeightmapTile> heightmaps;
	int vertex_cube_dimensions;

	void deleteHeightmap(glm::ivec2 coord);

public:
	ComputeShader heightmap_generator;
	Heightmap(int vertex_cube_dimensions, const char* heightmap_shader);
	~Heightmap();
	void generateHeightmap(glm::ivec2 coord);
	bool isGenerated(glm::ivec2 coord);
	GLuint getHeightmap(glm::ivec2 coord);
	void releaseHeightmap(glm::ivec2 coord);
};

