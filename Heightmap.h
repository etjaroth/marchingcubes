#pragma once
#include "config.h"

class Heightmap
{
	std::unordered_map<triple<int>, GLuint, tripleHashFunction> heightmaps;
	std::unordered_map<triple<int>, GLuint, tripleHashFunction> fences;

	void generate_heightmap(glm::ivec2 coord);
	bool is_generated(glm::ivec2 coord);
	GLuint get_heightmap(glm::ivec2);
};

