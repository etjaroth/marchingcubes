#include "ChunkManager.h"
#include <unordered_set>

const int buffer = 2;

ChunkManager::ChunkManager(unsigned int chunk_sz, glm::vec3 orgin, int r, const char* heightmap_shader, const char* fill_shader, float scale) :
	gen_verticies("genVerticies.comp"),
	gen_indicies("genIndices.comp"),
	heightmap_generator(chunk_sz + buffer, heightmap_shader),
	fill_generator(fill_shader, chunk_sz + buffer, chunk_sz + buffer, chunk_sz + buffer),
	lightingCalculator("lightingCalculator.comp", chunk_sz + buffer, 1, chunk_sz + buffer),
	model{ glm::mat4(scale) },
	invModel{ glm::inverse(model) } {

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
	position = invModel * glm::vec4(pos, 1.0f);

	glm::ivec3 old_chunk_pos = chunk_position;
	// Find location as a chunk
	unsigned int real_chunk_size = chunk_size + 1;
	real_chunk_size = chunk_size;
	glm::vec3 chunkSizeVec = model * glm::vec4(glm::vec3(real_chunk_size), 1.0f);

	chunk_position.x = (int)((position.x) / chunkSizeVec.x);
	chunk_position.y = (int)((position.y) / chunkSizeVec.y);
	chunk_position.z = (int)((position.z) / chunkSizeVec.z);

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
	chunks.clear();
}

void ChunkManager::set_direction(glm::vec3 dir) {
	direction = dir;
}

void ChunkManager::render(Shader* shader, double time, double dayNightSpeed) {
	shader->use();
	shader->setFloat("wavetime", 2.0f * (float)time);
	shader->setMat4("model", model);

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

	std::vector<std::pair<glm::ivec3, std::shared_ptr<Chunk>>> chunk_list(chunks.begin(), chunks.end());

	// Sort chunks by distance to the player so that closer chunks are loaded first
	std::sort(chunk_list.begin(), chunk_list.end(),
		[this](const std::pair<glm::ivec3, std::shared_ptr<Chunk>>& a,
			const std::pair<glm::ivec3, std::shared_ptr<Chunk>>& b) -> bool
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


	for (std::vector<std::pair<glm::ivec3, std::shared_ptr<Chunk>>>::iterator chunk = chunk_list.begin(); chunk != chunk_list.end(); chunk++) {
		const glm::vec3 p = model * glm::vec4(glm::vec3((int)chunk_size * chunk->first), 1.0f);
		const double distance = glm::length(position - p);
		chunk->second->render(shader, distance);
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

				if (chunks.find(point) == chunks.end()) {
					glm::ivec3 offset3 = static_cast<int>(chunk_size) * offset2;
					float cutoffDistance = glm::sqrt(3.0) * (model * glm::vec4(glm::vec3(100.0), 1.0f)).x;
					chunks.insert(
						std::pair<glm::ivec3, std::shared_ptr<Chunk>>(point,
							std::make_shared<Chunk>(
								cutoffDistance,
								chunk_size,
								offset3,
								heightmap_generator,
								fill_generator,
								lightingCalculator,
								gen_indicies,
								gen_verticies)));
				}
			}
		}
	}


	// Create/Destroy MarchingCubes at legal/illegal points
	for (auto chunk = chunks.begin(); chunk != chunks.end();) {
		glm::ivec3 point = chunk->first;

		if (legal_points.find(point) != legal_points.end()) {
			++chunk;
		}
		else {
			chunk = chunks.erase(chunk);
		}
	}
}