#include "DistantLandscape.h"
#include "Fence.h"

glm::ivec2 getCoord(unsigned int idx, int width, int objSize) {
	glm::ivec2 coord;
	coord.x = idx / (width * objSize);
	coord.y = idx % (width * objSize);
	return coord;
}

unsigned int getIndex(glm::ivec2 coord, int width, int objSize) {
	const unsigned int row = coord.x * objSize;
	const unsigned int column = coord.y * width * objSize;
	return row + column;
}

glm::vec4 getArrayElement(glm::ivec2 coord, int width, float* pixels) {
	const unsigned int row = coord.x * 4;
	const unsigned int column = coord.y * width * 4;
	const unsigned int idx = getIndex(coord, width, 4);
	const unsigned int px = idx + 0;
	const unsigned int py = idx + 1;
	const unsigned int pz = idx + 2;
	const unsigned int pw = idx + 3;
	return glm::vec4(pixels[px], pixels[py], pixels[pz], pixels[pw]);
}

struct DistantLandscape::Vertex {
	glm::vec4 pos;
	glm::vec4 normal;
	glm::vec4 material;
};

DistantLandscape::DistantLandscape(int _vertexCubeDimensions, glm::ivec2 pos, Heightmap& heightmapGenerator, SSBOComputeShader& genVerticies)
	: vertexCubeDimensions{ _vertexCubeDimensions + 2 },
	pos{ pos },
	heightmapGenerator{ heightmapGenerator },
	genVerticies{ genVerticies } {

	// Gen heightmap
	Fence fence{ true };
	heightmapGenerator.generateHeightmap(pos);
	HEIGHTMAP = heightmapGenerator.getHeightmap(pos);
	fence.waitUntilDone();
	bool b = fence.isDone();
	fence.release();

	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			continue;
		}
	}

	// Read Heightmap
	GLuint PBO = 0;
	glGenBuffers(1, &PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, HEIGHTMAP);

	const unsigned int size = 4 * (vertexCubeDimensions) * (vertexCubeDimensions);
//	const unsigned int size = 4 * (vertexCubeDimensions + 2) * (vertexCubeDimensions + 2);
	float* pixels = new float[size];

	glGetTextureImage(HEIGHTMAP, 0, GL_RGBA, GL_FLOAT, size * sizeof(float), pixels);
	fence.set();
	fence.waitUntilDone();
	fence.release();
	glDeleteBuffers(1, &PBO);


 	for (int x = 0; x <= vertexCubeDimensions; ++x) {
		for (int z = 0; z <= vertexCubeDimensions; ++z) {
			const glm::vec4 vec = getArrayElement(glm::ivec2(x, z), vertexCubeDimensions, pixels);
			const float y = vec.y;
			const float waterlevel = vec.w;

			glm::vec4 position{ x, y, z, 1.0f };
			position += glm::vec4(pos.x, 0, pos.y, 0);
			glm::vec4 normal{ 0.0f, 0.0f, 0.0f, 0.0f };

			glm::vec4 material;
			material.w = 1.0;

			if (y > waterlevel) {
				material.x = 1.0f; // grass
			}
			else {
				material.x = 0.0f; // water
				position.y = 0.0f;
			}

			verticies.push_back({ position, normal, material });
		}
	}

	// EBO data
	for (int x = 1; x < vertexCubeDimensions; ++x) {
		for (int z = 1; z < vertexCubeDimensions; ++z) {
			unsigned int i = getIndex(glm::ivec2(x, z), vertexCubeDimensions, 1);
			indicies.push_back(i);
			indicies.push_back(getIndex(glm::ivec2(x - 1, z), vertexCubeDimensions, 1));
			indicies.push_back(getIndex(glm::ivec2(x, z - 1), vertexCubeDimensions, 1));

			indicies.push_back(getIndex(glm::ivec2(x - 1, z - 1), vertexCubeDimensions, 1));
			indicies.push_back(getIndex(glm::ivec2(x, z - 1), vertexCubeDimensions, 1));
			indicies.push_back(getIndex(glm::ivec2(x - 1, z), vertexCubeDimensions, 1));
		}
	}

	// Get normals
	for (int i = 0; i < indicies.size(); i += 3) {
		glm::vec3 orgin = verticies[indicies[i + 0]].pos;
		glm::vec3 cross1 = glm::vec3(verticies[indicies[i + 1]].pos) - orgin;
		glm::vec3 cross2 = glm::vec3(verticies[indicies[i + 2]].pos) - orgin;

		//std::cout << glm::to_string(orgin) << std::endl;


		glm::vec4 normal = glm::vec4(glm::normalize(glm::cross(cross1, cross2)), 1.0);

		verticies[indicies[i + 0]].normal = normal;
		verticies[indicies[i + 1]].normal = normal;
		verticies[indicies[i + 2]].normal = normal;
	}


	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Vertex Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Vertex Normal
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);
	// Vertex Material
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, material)));
	glEnableVertexAttribArray(2);

	// Buffer Verticies
	glBufferData(GL_ARRAY_BUFFER, verticies.size(), &verticies[0], GL_STATIC_DRAW);

	// EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size(), &indicies[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] pixels;
}

DistantLandscape::~DistantLandscape() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	heightmapGenerator.releaseHeightmap(pos);
}

void DistantLandscape::renderCubes(Shader* shader) {
	// Draw
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	shader->use();
	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
	shader->dontuse();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}