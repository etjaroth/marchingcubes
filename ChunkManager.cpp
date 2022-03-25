#include "ChunkManager.h"
#include <unordered_set>

const int buffer = 2;

ChunkManager::ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* heightmap_shader, const char* fill_shader) :
	gen_verticies("genVerticies.comp"),
	gen_indicies("genIndices.comp"),
	heightmap_generator(chunk_sz + buffer, heightmap_shader),
	fill_generator(fill_shader, chunk_sz + buffer, chunk_sz + buffer, chunk_sz + buffer),
	lightingCalculator("lightingCalculator.comp", chunk_sz + buffer, 1, chunk_sz + buffer) {

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
	closeTerrain.clear();
	farTerrain.clear();
}

void ChunkManager::set_direction(glm::vec3 dir) {
	direction = dir;
}

void ChunkManager::render(Shader* shader, double time, double dayNightSpeed) {
	shader->setFloat("wavetime", 2.0f * (float)time);

	// Calculate brightness
	{
		const double minBrightness = 0.2;
		double brightness = glm::sin(-time * dayNightSpeed);
		if (brightness + 0.1 > 0.0) { // should still be a little bright over the horizen
			double a = 80.0 * (brightness + 0.1);
			brightness = 1.0 / (a * a + 1.0); // applies abs, peaks near 0 (sunrise/sunset)
			brightness = std::max(1.0 - brightness, 0.2);
		}
		else {
			brightness = minBrightness;
		}

		shader->setFloat("brightness", static_cast<float>(brightness));
	}

	//shader->setFloat("brightness", std::max(0.2f, std::max(-(float)glm::sin(dayNightSpeed * time), 0.0f)));

	std::vector<std::pair<glm::ivec3, std::shared_ptr<MarchingCubes>>> chunk_list(closeTerrain.begin(), closeTerrain.end());

	// Sort chunks by distance to the player so that closer chunks are loaded first
	std::sort(chunk_list.begin(), chunk_list.end(),
		[this](const std::pair<glm::ivec3, std::shared_ptr<MarchingCubes>>& a,
			const std::pair<glm::ivec3, std::shared_ptr<MarchingCubes>>& b) -> bool
		{
			glm::vec3 fa = glm::vec3(a.first);
			glm::vec3 fb = glm::vec3(b.first);

			// Define order
			float arrA[3] = { fa.x, fa.z, fa.y };
			float arrB[3] = { fb.x, fb.z, fb.y };

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
				}
			}
			return false;
		});


	for (std::vector<std::pair<glm::ivec3, std::shared_ptr<MarchingCubes>>>::iterator chunk = chunk_list.begin(); chunk != chunk_list.end(); chunk++) {
		chunk->second->renderCubes(shader);
	}

}

void ChunkManager::update_chunks() {
	// List legal points
	std::unordered_set<glm::ivec3> legal_points;

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

	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			for (int z = -radius; z <= radius; z++) {
				glm::ivec3 point{
					x + chunk_position.x + offset.x,
					y + chunk_position.y + offset.y,
					z + chunk_position.z + offset.z };

				glm::ivec3 offset2 = glm::ivec3(point);

				legal_points.insert(point);

				if (closeTerrain.find(point) == closeTerrain.end()) {
					glm::ivec3 offset3 = static_cast<int>(chunk_size) * offset2;
					closeTerrain.insert(std::pair<glm::ivec3,
						std::shared_ptr<MarchingCubes>>(point,
							std::make_shared<MarchingCubes>(chunk_size, offset3, &heightmap_generator, &fill_generator, &lightingCalculator, &gen_indicies, &gen_verticies)));
				}
			}
		}
	}


	// Create/Destroy MarchingCubes at legal/illegal points
	for (auto chunk = closeTerrain.begin(); chunk != closeTerrain.end();) {
		glm::ivec3 point = chunk->first;

		if (legal_points.find(point) != legal_points.end()) {
			++chunk;
		}
		else {
			chunk = closeTerrain.erase(chunk);
		}
	}
}