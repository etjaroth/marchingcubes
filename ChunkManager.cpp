#include "ChunkManager.h"
#include <unordered_set>

ChunkManager::ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* landscape_generator) :
	gen_verticies("genVerticies.comp"),
	fill_generator(landscape_generator, chunk_sz + 1, chunk_sz + 1, chunk_sz + 1) {

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

void ChunkManager::set_direction(glm::vec3 dir) {
	direction = dir;
}

void ChunkManager::render(Shader* shader) {
	for (std::unordered_map<triple<int>, std::unique_ptr<MarchingCubes>, tripleHashFunction>::iterator chunk = chunk_map.begin(); chunk != chunk_map.end(); chunk++) {

		// Check if chunk is visable
		bool corner_visable = false;
		float angle = 0.0f;

		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f,       0.0f,       0.0f))       >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, 0.0f,       0.0f))       >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f,       chunk_size, 0.0f))       >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, chunk_size, 0.0f))       >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f,       0.0f,       chunk_size)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, 0.0f,       chunk_size)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f,       chunk_size, chunk_size)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, chunk_size, chunk_size)) >= angle;


		if (corner_visable) {
			chunk->second->renderCubes(shader);
		}
	}
}

void ChunkManager::update_chunks() {

	// List legal points
	std::unordered_set<triple<int>, tripleHashFunction> legal_points;
	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			for (int z = -radius; z <= radius; z++) {
				triple<int> point =
				{ {x + chunk_position.x,
					y + chunk_position.y,
					z + chunk_position.z} };

				glm::ivec3 offset = glm::ivec3(point.three[0], point.three[1], point.three[2]);
				legal_points.insert(point);

				int a = x + chunk_position.x - radius; // x + chunk_position.x - num * radius
				int b = y + chunk_position.y - radius; // y + chunk_position.y - num * radius
				int c = z + chunk_position.z - radius; // z + chunk_position.z - num * radius

				if (chunk_map.find(point) == chunk_map.end()) {
					offset *= chunk_size;
					chunk_map.insert(std::pair<triple<int>, std::unique_ptr<MarchingCubes>>(point, std::make_unique<MarchingCubes>(chunk_size, offset, &gen_verticies, &fill_generator)));
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
	std::cout << legal_points.size() << std::endl;
}