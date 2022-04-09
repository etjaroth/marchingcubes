#include "DistantLandscape.h"
#include "Fence.h"
#include "HeightmapMeshGenerator.h"

glm::ivec2 getCoord(unsigned int idx, int width, int objSize) {
	glm::ivec2 coord;
	coord.x = idx / (width * objSize);
	coord.y = idx % (width * objSize);
	return coord;
}

unsigned int getIndex(glm::ivec2 coord, int width, int objSize) {
	const unsigned int row = coord.x * objSize;
	const unsigned int column = coord.y * (width)*objSize;
	return row + column;
}

glm::vec4 getArrayElement(glm::ivec2 coord, int width, float* pixels) {
	const unsigned int idx = getIndex(coord, width, 4);
	const unsigned int px = idx + 0;
	const unsigned int py = idx + 1;
	const unsigned int pz = idx + 2;
	const unsigned int pw = idx + 3;

	const float fx = pixels[px];
	const float fy = pixels[py];
	const float fz = pixels[pz];
	const float fw = pixels[pw];

	return glm::vec4(fx, fy, fz, fw);
}

struct DistantLandscape::Vertex {
	glm::vec4 pos;
	glm::vec4 normal;
	glm::vec4 material;
};

DistantLandscape::DistantLandscape(int _vertexCubeDimensions, glm::ivec3 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies)
	: vertexCubeDimensions{ _vertexCubeDimensions + 2 },
	pos{ pos },
	heightmapGenerator{ heightmapGenerator },
	genVerticies{ genVerticies } {

	// Gen heightmap
	Fence fence{ true };
	heightmapGenerator.generateHeightmap(glm::ivec2(pos.x, pos.z));
	HEIGHTMAP = heightmapGenerator.getHeightmap(glm::ivec2(pos.x, pos.z));
	fence.waitUntilDone();
	bool b = fence.isDone();
	fence.release();
}

DistantLandscape::~DistantLandscape() {
	if (PBO) {
		glDeleteBuffers(1, &PBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		delete[] pixels;
		heightmapGenerator.releaseHeightmap(glm::ivec2(pos.x, pos.z));
	}
}

void DistantLandscape::renderCubes(Shader* shader) {

	switch (currentStep) {
	case RenderingStages::genHeightmap:
		loadHeightmap();
		break;
	case RenderingStages::genVerticies:
		generate();
		break;
	case RenderingStages::done:
		// Draw
		if (mesh) {
			mesh->render(*shader);
		}
		break;
	default:
		break;
	}
}

void DistantLandscape::loadHeightmap() {
	if (!fence.isActive()) {
		// Read Heightmap
		glGenBuffers(1, &PBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HEIGHTMAP);

		const unsigned int size = 4 * (vertexCubeDimensions) * (vertexCubeDimensions);
		pixels = new float[size];

		glGetTextureImage(HEIGHTMAP, 0, GL_RGBA, GL_FLOAT, size * sizeof(float), pixels);
		fence.set();
	}

	if (!fence.isDone()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		return;
	}


	fence.release();
	glDeleteBuffers(1, &PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	PBO = 0;
	currentStep = RenderingStages::genVerticies;
	generate();
}

void DistantLandscape::generate() {
	// generate in HeightmapMeshGenerator
	
	HeightmapMeshGenerator gen{vertexCubeDimensions, pixels, pos};
	gen();
	mesh = gen.getData();
	
	// generate in HeightmapMeshGenerator

	delete[] pixels;

	if (mesh) {
		currentStep = RenderingStages::done;
	}
	else {
		currentStep = RenderingStages::empty;
	}

	
}