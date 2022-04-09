#include "HeightmapMeshGenerator.h"

glm::ivec2 HeightmapMeshGenerator::getCoord(unsigned int idx, int width, int objSize) {
	glm::ivec2 coord;
	coord.x = idx / (width * objSize);
	coord.y = idx % (width * objSize);
	return coord;
}

unsigned int HeightmapMeshGenerator::getIndex(glm::ivec2 coord, int width, int objSize) {
	const unsigned int row = coord.x * objSize;
	const unsigned int column = coord.y * (width)*objSize;
	return row + column;
}

glm::vec4 HeightmapMeshGenerator::getArrayElement(glm::ivec2 coord, int width, float* pixels) {
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

HeightmapMeshGenerator::HeightmapMeshGenerator(int vertexCubeDimensions, float* pixels, glm::vec3 pos)
	: 
	//AbstractThreadTaskTemplate<std::shared_ptr<TerrainMesh>>(),
	vertexCubeDimensions{ vertexCubeDimensions },
	pixels{pixels},
	pos{pos} {

}

HeightmapMeshGenerator::~HeightmapMeshGenerator() {

}

void HeightmapMeshGenerator::operator()() {
	std::vector<TerrainMesh::Vertex> verticies;
	std::vector<unsigned int> indicies;

	float minHeight = 0.0f;
	float maxHeight = 0.0f;
	bool firstChecked = true;
	for (unsigned int x = 0; x < vertexCubeDimensions; ++x) {
		for (unsigned int z = 0; z < vertexCubeDimensions; ++z) {
			const glm::vec4 vec = getArrayElement(glm::ivec2(x, z), vertexCubeDimensions, pixels);
			const float y = vec.y;
			const float waterlevel = vec.w - 1.0f; // gen water at the bottom of the cube

			glm::vec4 position{ x, y, z, 1.0f };
			position += glm::vec4(pos.x, 0, pos.z, 0);
			glm::vec4 normal{ 0.0f, 0.0f, 0.0f, 0.0f };

			glm::vec4 material = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			if (y > waterlevel) {
				material.x = 1.0f; // grass
			}
			else {
				material.x = 0.0f; // water
				position.y = waterlevel;
			}

			if (firstChecked) {
				firstChecked = false;
				minHeight = position.y;
				maxHeight = position.y;
			}

			minHeight = std::min(position.y, minHeight);
			maxHeight = std::max(position.y, maxHeight);

			// I don't know why we need that correction to the position
			verticies.push_back({ position + glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), material });
		}
	}

	const float bottomHeight = pos.y;
	const float topHeight = (pos.y + (vertexCubeDimensions - 2));
	const bool topInBox = (bottomHeight <= maxHeight && maxHeight <= topHeight); // top of mesh is in box
	const bool bottomInBox = (bottomHeight <= minHeight && minHeight <= topHeight); // bottom of mesh is in box
	const bool tallMesh = (minHeight <= bottomHeight && topHeight <= maxHeight); // mesh is taller than box

	bool containsMesh = topInBox || bottomInBox || tallMesh;

	if (containsMesh) {
		// EBO data
		for (unsigned int x = 1; x <= vertexCubeDimensions / 2; ++x) {
			for (unsigned int z = 1; z <= vertexCubeDimensions / 2; ++z) {
				unsigned int i[6] = {
					getIndex(2 * glm::ivec2(x, z), vertexCubeDimensions, 1),
					getIndex(2 * glm::ivec2(x - 1, z), vertexCubeDimensions, 1),
					getIndex(2 * glm::ivec2(x, z - 1), vertexCubeDimensions, 1),

					getIndex(2 * glm::ivec2(x - 1, z - 1), vertexCubeDimensions, 1),
					getIndex(2 * glm::ivec2(x, z - 1), vertexCubeDimensions, 1),
					getIndex(2 * glm::ivec2(x - 1, z), vertexCubeDimensions, 1)
				};

				indicies.push_back(i[0]);
				indicies.push_back(i[1]);
				indicies.push_back(i[2]);
				indicies.push_back(i[3]);
				indicies.push_back(i[4]);
				indicies.push_back(i[5]);
			}
		}

		// Get normals
		for (int i = 0; i < indicies.size(); i += 3) {
			glm::vec3 orgin = verticies[indicies[i + 0]].pos;
			glm::vec3 cross1 = glm::vec3(verticies[indicies[i + 1]].pos) - orgin;
			glm::vec3 cross2 = glm::vec3(verticies[indicies[i + 2]].pos) - orgin;

			glm::vec4 normal = glm::vec4(glm::normalize(glm::cross(cross1, cross2)), 1.0);

			verticies[indicies[i + 0]].normal = normal;
			verticies[indicies[i + 1]].normal = normal;
			verticies[indicies[i + 2]].normal = normal;
		}

		//mesh = std::make_shared<TerrainMesh>(verticies, indicies);
		data = std::make_shared<TerrainMesh>(verticies, indicies);
	}
	else {

		// do nothing

		//mesh = nullptr;
		data = nullptr;
	}
	
}