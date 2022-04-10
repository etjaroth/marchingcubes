#include "Chunk.h"

Chunk::Chunk(unsigned int cutoffDistance, int cubeSize, glm::ivec3 position, Heightmap& heightmap_generator_ptr, ComputeShader& fill_generator_ptr, ComputeShader& lightingCalculatorPtr, SSBOComputeShader& gen_indices_ptr, SSBOComputeShader& gen_verticies_ptr, std::shared_ptr<ThreadManager> threadManager)
	: cutoffDistance{ cutoffDistance } {
	highDetail = new MarchingCubes(cubeSize, position, heightmap_generator_ptr, fill_generator_ptr, lightingCalculatorPtr, gen_indices_ptr, gen_verticies_ptr);
	lowDetail = new DistantLandscape(cubeSize, position, heightmap_generator_ptr, gen_verticies_ptr, threadManager);
}

Chunk::~Chunk() {
	delete highDetail;
	delete lowDetail;
}

Chunk::Chunk(Chunk&& other) noexcept
	: cutoffDistance{ other.cutoffDistance },
	highDetail{ std::exchange(other.highDetail, nullptr) },
	lowDetail{ std::exchange(other.lowDetail, nullptr) }
{}

Chunk& Chunk::operator=(Chunk&& other) noexcept {
	cutoffDistance = other.cutoffDistance;
	std::swap(highDetail, other.highDetail);
	std::swap(lowDetail, other.lowDetail);
	return *this;
}

void Chunk::render(Shader* shader, double distance) {
	bool b = distance <= cutoffDistance;
	//b = false;

	if (b) {
		highDetail->renderCubes(shader);
		// Draw the low detail version if the complex version isn't ready
		if (!highDetail->isDone()) {
			lowDetail->renderCubes(shader);
		}
	}
	else {
		lowDetail->renderCubes(shader);
	}
}