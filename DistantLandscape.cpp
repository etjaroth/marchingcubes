#include "DistantLandscape.h"

DistantLandscape::DistantLandscape(glm::ivec2 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies)
	: pos{ pos },
	heightmapGenerator{ heightmapGenerator },
	genVerticies{ genVerticies } {

	// 
	heightmapGenerator.generate_heightmap(pos);
	while (!heightmapGenerator.is_generated(pos)) {
		// Wait
	}

}

DistantLandscape::~DistantLandscape() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	heightmapGenerator.release_heightmap(pos);
}

void DistantLandscape::renderCubes(Shader* shader) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	// Draw
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	shader->use();
	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
	shader->dontuse();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}