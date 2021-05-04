#include "ChunkManager.h"
#include <unordered_set>

ChunkManager::ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* heightmap_shader, const char* fill_shader) :
	gen_verticies("genVerticies.comp"),
	heightmap_generator(heightmap_shader, chunk_sz + 1, 1, chunk_sz + 1),
	fill_generator(fill_shader, chunk_sz + 1, chunk_sz + 1, chunk_sz + 1) {

	chunk_size = chunk_sz;
	set_pos(orgin);
	radius = r;

	heightmap_generator.use();
	heightmap_generator.setVec3("boundryA", glm::vec3(10.0f, 0.0f, 1.0f));
	heightmap_generator.setVec3("boundryB", glm::vec3(100.0f, 0.0f, 1.0f));
	heightmap_generator.setuInt("biome_type_A", 0);
	heightmap_generator.setuInt("biome_type_B", 0);
	heightmap_generator.dontuse();

	fill_generator.use();
	fill_generator.setVec3("boundryA", glm::vec3(10.0f, 0.0f, 1.0f));
	fill_generator.setVec3("boundryB", glm::vec3(100.0f, 0.0f, 1.0f));
	fill_generator.setuInt("biome_type_A", 0);
	fill_generator.setuInt("biome_type_B", 0);
	fill_generator.dontuse();

	update_chunks();
}

// get sign of number
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void ChunkManager::set_pos(glm::vec3 pos) {
	glm::vec3 oldpos = position;
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
	else if (sgn<float>(oldpos.x) != sgn<float>(position.x) ||
		sgn<float>(oldpos.y) != sgn<float>(position.y) ||
		sgn<float>(oldpos.z) != sgn<float>(position.z)) { // account for 0 not being signed (giant cell at (0, 0, 0))
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

		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f, 0.0f, 0.0f)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, 0.0f, 0.0f)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f, chunk_size, 0.0f)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, chunk_size, 0.0f)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f, 0.0f, chunk_size)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, 0.0f, chunk_size)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(0.0f, chunk_size, chunk_size)) >= angle;
		corner_visable = corner_visable || glm::dot(chunk->second->getPos() - position, direction - glm::vec3(chunk_size, chunk_size, chunk_size)) >= angle;

		corner_visable = true;


		if (corner_visable) {
			chunk->second->renderCubes(shader);
		}
		else {
		//std::cout << "-";
		}
	}
	//std::cout << std::endl;
}

void ChunkManager::update_chunks() {

	// List legal points
	std::unordered_set<triple<int>, tripleHashFunction> legal_points;

	// Account for the giant cell at (0, 0, 0)
	glm::vec3 offset = glm::vec3(0.0f);
	if (position.x < 0) {
		offset.x = -1.0f;
	}
	else {
		offset.x = 0.0;
	}

	if (position.y < 0) {
		offset.y = -1.0f;
	}
	else {
		offset.y = 0.0;
	}

	if (position.z < 0) {
		offset.z = -1.0f;
	}
	else {
		offset.z = 0.0;
	}

	/*std::cout << "Coord: " << chunk_position.x + offset.x << ", " << chunk_position.y + offset.y << ", " << chunk_position.z + offset.z << std::endl;
	std::cout << "       " << chunk_position.x << ", " << chunk_position.y << ", " << chunk_position.z << std::endl;
	std::cout << "       " << offset.x << ", " << offset.y << ", " << offset.z << std::endl;
	std::cout << "----------" << std::endl;*/

	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			for (int z = -radius; z <= radius; z++) {
				triple<int> point =
				{ {x + chunk_position.x + offset.x,
					y + chunk_position.y + offset.y,
					z + chunk_position.z + offset.z} };

				//if (y == 0)
					//std::cout << x + chunk_position.x + offset.x << ", " << y + chunk_position.y + offset.y << ", " << z + chunk_position.z + offset.z << std::endl;

				glm::ivec3 offset = glm::ivec3(point.three[0], point.three[1], point.three[2]);

				legal_points.insert(point);

				if (chunk_map.find(point) == chunk_map.end()) {
					offset *= chunk_size;
					chunk_map.insert(std::pair<triple<int>, 
						std::unique_ptr<MarchingCubes>>(point, 
							std::make_unique<MarchingCubes>(chunk_size, offset, &heightmap_generator, &fill_generator, &gen_verticies)));
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
	//std::cout << legal_points.size() << std::endl;
}