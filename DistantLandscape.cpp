#include "DistantLandscape.h"

DistantLandscape::DistantLandscape(glm::ivec3 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies)
	: pos{pos},
	heightmapGenerator {
	heightmapGenerator
},
	genVerticies{ genVerticies } {

}

DistantLandscape::~DistantLandscape() {
	glDeleteBuffers(1, &INDIRECT_SSBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VERTEX_SSBO);
	glDeleteVertexArrays(1, &VAO);
	heightmapGenerator.release_heightmap(glm::ivec2(pos.x, pos.z));

	fence.release();
}

void DistantLandscape::renderCubes(Shader* shader) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	// Draw
	glBindVertexArray(VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
	glBindBuffer(GL_ARRAY_BUFFER, VERTEX_SSBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	shader->use();
	glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
	shader->dontuse();


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}