#include "ChunkManager.h"
#include <unordered_set>

//const int buffer = 2;
const int buffer = 0;

ChunkManager::ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* heightmap_shader, const char* fill_shader) :
	gen_verticies("genVerticies.comp"),
	gen_indicies("genIndices.comp"),
	heightmap_generator(chunk_sz + buffer, heightmap_shader),
	fill_generator(fill_shader, chunk_sz + buffer, chunk_sz + buffer, chunk_sz + buffer) {

	chunk_size = chunk_sz;
	set_pos(orgin);
	radius = r;

	// To be depricated
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

ChunkManager::~ChunkManager() {
	// Marching cubes need to be destructed before heightmap_generator
	chunk_map.clear();
}

void ChunkManager::set_direction(glm::vec3 dir) {
	direction = dir;
}

void ChunkManager::render(Shader* shader) {
	std::cout << '[' << std::endl;


	std::vector<std::pair<triple<int>, std::shared_ptr<MarchingCubes>>> chunk_list(chunk_map.begin(), chunk_map.end());

	// Sort chunks by distance to the player so that closer chunks are loaded first
	std::sort(chunk_list.begin(), chunk_list.end(),
		[this](const std::pair<triple<int>, std::shared_ptr<MarchingCubes>>& a, 
			const std::pair<triple<int>, std::shared_ptr<MarchingCubes>>& b) -> bool
		{
			/*const glm::vec3 va = (float)(this->chunk_size) * glm::vec3(a.first.three[0], a.first.three[1], a.first.three[2]);
			const glm::vec3 vb = (float)(this->chunk_size) * glm::vec3(b.first.three[0], b.first.three[1], b.first.three[2]);

			const glm::vec3 dist_a = va - this->position;
			const glm::vec3 dist_b = vb - this->position;

			return glm::length(dist_a)
				< glm::length(dist_b);*/

			glm::vec3 fa = glm::vec3(a.first.three[0], a.first.three[1], a.first.three[2]);
			glm::vec3 fb = glm::vec3(b.first.three[0], b.first.three[1], b.first.three[2]);

			float arrA[3] = {fa.y, fa.x, fa.z};
			float arrB[3] = {fb.y, fb.x, fb.z};

			if (arrA[0] < arrB[0]) {
				return true;
			}
			else if (arrA[0] == arrB[0]) {
				if (arrA[1] < arrB[1]) {
					return true;
				}
				else if (arrA[1] == arrB[1]) {
					if (arrA[2] < arrB[2]) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		});


	for (std::vector<std::pair<triple<int>, std::shared_ptr<MarchingCubes>>>::iterator chunk = chunk_list.begin(); chunk != chunk_list.end(); chunk++) {
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
		}
	}
	//std::cout << "==========" << std::endl;
	std::cout << ']' << std::endl;
}

void ChunkManager::update_chunks() {
	// List legal points
	std::unordered_set<triple<int>, tripleHashFunction> legal_points;

	//std::cout << "\n\nUpdating chunks: " << chunk_map.size() << std::endl;

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

	//std::cout << "Player is at: " << chunk_position.x << ", " << chunk_position.y << ", " << chunk_position.z << std::endl;
	//std::cout << "Legal Points: " << std::endl;
	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			for (int z = -radius; z <= radius; z++) {
				triple<int> point =
				{ {x + chunk_position.x + offset.x,
					y + chunk_position.y + offset.y,
					z + chunk_position.z + offset.z} };

				glm::ivec3 offset2 = glm::ivec3(point.three[0], point.three[1], point.three[2]);

				legal_points.insert(point);
				//std::cout << point.three[0] << ", " << point.three[1] << ", " << point.three[2] << std::endl;

				if (chunk_map.find(point) == chunk_map.end()) {
					//glm::ivec3 offset3 = static_cast<int>(chunk_size - 1) * offset2; // temporary?
					glm::ivec3 offset3 = static_cast<int>(chunk_size) * offset2;
					chunk_map.insert(std::pair<triple<int>,
						std::shared_ptr<MarchingCubes>>(point,
							std::make_shared<MarchingCubes>(chunk_size, offset3, &heightmap_generator, &fill_generator, &gen_indicies, &gen_verticies)));
				}
			}
		}
	}


	// Create/Destroy MarchingCubes at legal/illegal points
	for (auto chunk = chunk_map.begin(); chunk != chunk_map.end();) {
		triple<int> point = chunk->first;

		if (legal_points.find(point) != legal_points.end()) {
			++chunk;
		}
		else {
			chunk = chunk_map.erase(chunk);
		}
	}

}