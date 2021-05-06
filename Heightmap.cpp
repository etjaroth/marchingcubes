#include "Heightmap.h"

struct heightmap_tile {
	GLuint texture;
	bool is_generated;
	GLsync fence;
	unsigned int refrence_count;
};

Heightmap::Heightmap(int vertex_cube_dimension, const char* heightmap_shader) : heightmap_generator(heightmap_shader, vertex_cube_dimension, 1, vertex_cube_dimension) {
	vertex_cube_dimensions = vertex_cube_dimension;

	// Set biomes for testing
	heightmap_generator.use();
	heightmap_generator.setVec3("boundryA", glm::vec3(10.0f, 0.0f, 1.0f));
	heightmap_generator.setVec3("boundryB", glm::vec3(100.0f, 0.0f, 1.0f));
	heightmap_generator.setuInt("biome_type_A", 0);
	heightmap_generator.setuInt("biome_type_B", 0);
	heightmap_generator.dontuse();
}

Heightmap::~Heightmap() {
	for (std::unordered_map<triple<int>, heightmap_tile, tripleHashFunction>::iterator i = heightmaps.begin(); i != heightmaps.end();) {
		std::unordered_map<triple<int>, heightmap_tile, tripleHashFunction>::iterator prev = i++;
		delete_heightmap(glm::ivec2(prev->first.three[0], prev->first.three[2]));
	}
}

void Heightmap::generate_heightmap(glm::ivec2 coord) {
	triple<int> key = { {coord.x, 0, coord.y} };

	auto itr = heightmaps.find(key);
	if (itr != heightmaps.end()) {
		return;
	}

	heightmap_tile hmap = { 0, false, glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0), 0 };
	glGenTextures(1, &hmap.texture);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, hmap.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, vertex_cube_dimensions, vertex_cube_dimensions, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(1, hmap.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	heightmap_generator.use();
	heightmap_generator.setVec3("offset", glm::vec3(coord.x, 0.0f, coord.y));
	heightmap_generator.fillTexture(hmap.texture);
	heightmap_generator.dontuse();

	heightmaps.insert({ key, hmap });
}

void Heightmap::delete_heightmap(glm::ivec2 coord) {
	std::unordered_map<triple<int>, heightmap_tile, tripleHashFunction>::iterator itr = heightmaps.find({ coord.x, 0, coord.y });
	if (itr == heightmaps.end()) {
		return;
	}

	if (!(itr->second.is_generated)) {
		glDeleteSync(itr->second.fence);
	}

	glDeleteTextures(1, &itr->second.texture);
	heightmaps.erase(itr);
}

bool Heightmap::is_generated(glm::ivec2 coord) {
	std::unordered_map<triple<int>, heightmap_tile, tripleHashFunction>::iterator itr = heightmaps.find({ coord.x, 0, coord.y });

	if (itr == heightmaps.end()) { // we haven't even started
		return false;
	}
	else if (itr->second.is_generated) { // we already know it's done
		return true;
	}
	else { // check if it's done
		GLint syncStatus[1] = { GL_UNSIGNALED };
		glGetSynciv(itr->second.fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
		bool finished = syncStatus[0] == GL_SIGNALED;

		if (finished) {
			itr->second.is_generated = true;
			glDeleteSync(itr->second.fence);
		}

		return finished;
	}
}

GLuint Heightmap::get_heightmap(glm::ivec2 coord) {
	auto itr = heightmaps.find({ coord.x, 0, coord.y });
	if (itr == heightmaps.end()) {
		return 0;
	}
	else {
		itr->second.refrence_count += 1;
		return itr->second.texture;
	}
}

void Heightmap::release_heightmap(glm::ivec2 coord) {
	auto itr = heightmaps.find({ coord.x, 0, coord.y });
	if (itr != heightmaps.end()) {
		itr->second.refrence_count -= 1;
		if (itr->second.refrence_count <= 0) {
			delete_heightmap(coord);
		}
	}
}