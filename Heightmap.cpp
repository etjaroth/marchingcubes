#include "Heightmap.h"

void Heightmap::generate_heightmap(glm::ivec2 coord) {
	if (heightmaps.find({ {coord.x, 0, coord.y} }) == heightmaps.end()) {
		return;
	}


}

bool Heightmap::is_generated(glm::ivec2 coord) {

}

GLuint Heightmap::get_heightmap(glm::ivec2) {

}