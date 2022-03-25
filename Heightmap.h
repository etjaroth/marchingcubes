#pragma once
#include "Config.h"
#include "ComputeShader.h"
struct HeightmapTile;


class Heightmap {
	std::unordered_map<glm::ivec3, HeightmapTile> heightmaps;
	int vertex_cube_dimensions;

	void delete_heightmap(glm::ivec2 coord);

public:
	ComputeShader heightmap_generator;
	Heightmap(int vertex_cube_dimensions, const char* heightmap_shader);
	~Heightmap();
	void generate_heightmap(glm::ivec2 coord);
	bool is_generated(glm::ivec2 coord);
	GLuint get_heightmap(glm::ivec2 coord);
	void release_heightmap(glm::ivec2 coord);
};

