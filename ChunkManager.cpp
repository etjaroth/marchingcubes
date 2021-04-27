#include "ChunkManager.h"
#include <unordered_set>

ChunkManager::ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, unsigned int r, const char* landscape_generator) {

	chunk_size = chunk_sz;
	set_pos(orgin);
	radius = r;
	shader_file = landscape_generator;
	update_chunks();
}

void ChunkManager::set_pos(glm::vec3 pos) {
	position = pos;

	glm::ivec3 old_chunk_pos = chunk_position;
	// Find location as a chunk
	unsigned int real_chunk_size = chunk_size + 1;
	real_chunk_size = chunk_size;

	chunk_position.x = (int)((position.x) / real_chunk_size);
	chunk_position.y = (int)((position.y) / real_chunk_size);
	chunk_position.z = (int)((position.z) / real_chunk_size);

	if (old_chunk_pos != chunk_position) {
		update_chunks();
	}
}

void ChunkManager::render(Shader* shader) {
	for (std::unordered_map<triple<int>, std::unique_ptr<MarchingCubes>, tripleHashFunction>::iterator chunk = chunk_map.begin(); chunk != chunk_map.end(); chunk++) {
		chunk->second->renderCubes(shader);
	}
}

void ChunkManager::update_chunks() {
	int diameter = 2 * radius + 1; // (including a point at 0)

	// List legal points
	std::unordered_set<triple<int>, tripleHashFunction> legal_points;
	for (int x = 0; x < diameter; x++) {
		for (int y = 0; y < diameter; y++) {
			for (int z = 0; z < diameter; z++) {
				const int c = 2;
				triple<int> point =
				{ {x + chunk_position.x - c * radius,
					y + chunk_position.y - c * radius,
					z + chunk_position.z - c * radius} };

				glm::ivec3 offset = glm::ivec3(point.three[0], point.three[1], point.three[2]);
				legal_points.insert(point);

				if (chunk_map.find(point) == chunk_map.end()) {
					offset *= chunk_size;
					chunk_map.insert(std::pair<triple<int>, std::unique_ptr<MarchingCubes>>(point, std::make_unique<MarchingCubes>(chunk_size, offset, shader_file)));
				}
			}
		}
	}

	// Create/Destroy MarchingCubes at legal/illegal points
	for (auto chunk = chunk_map.begin(); chunk != chunk_map.end();) {
		triple<int> point = chunk->first;
		glm::ivec3 offset = glm::ivec3(point.three[0], point.three[1], point.three[2]);
		offset *= chunk_size;

		if (legal_points.find(point) != legal_points.end()) {
			++chunk;
		}
		else {
			chunk = chunk_map.erase(chunk);
		}
	}
}