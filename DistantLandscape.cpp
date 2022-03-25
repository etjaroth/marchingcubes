#include "DistantLandscape.h"

DistantLandscape::DistantLandscape(int vertexCubeDimensions, glm::ivec2 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies)
	: vertexCubeDimensions{ vertexCubeDimensions },
	pos{ pos },
	heightmapGenerator{ heightmapGenerator },
	genVerticies{ genVerticies } {

	// 
	heightmapGenerator.generate_heightmap(pos);
	while (!heightmapGenerator.is_generated(pos)) {
		// Wait
	}

	glBindBuffer(GL_TEXTURE_2D, HEIGHTMAP);
	float* pixels = new float[4 * vertexCubeDimensions * vertexCubeDimensions];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &pixels);
	// elevation is in y
	// watertable is in w
	for (int y = 0; y < vertexCubeDimensions; ++y) {
		for (int x = 0; x < vertexCubeDimensions; ++x) {
			const unsigned int row = x * 4;
			const unsigned int column = y * vertexCubeDimensions * 4;
			const unsigned int px = row + column + 0;
			const unsigned int py = row + column + 1;
			const unsigned int pz = row + column + 2;
			const unsigned int pw = row + column + 3;
			std::cout << "[" << pixels[px] << ", " << pixels[py] << ", " << pixels[pz] << ", " << pixels[pw] << "], ";
		}
		std::cout << std::endl;
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