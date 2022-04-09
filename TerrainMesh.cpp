#include "TerrainMesh.h"

void TerrainMesh::setupVAO() {
	// Vertex Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TerrainMesh::Vertex), (void*)(offsetof(TerrainMesh::Vertex, pos)));
	glEnableVertexAttribArray(0);
	// Vertex Normal
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TerrainMesh::Vertex), (void*)(offsetof(TerrainMesh::Vertex, normal)));
	glEnableVertexAttribArray(1);
	// Vertex Material
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TerrainMesh::Vertex), (void*)(offsetof(TerrainMesh::Vertex, material)));
	glEnableVertexAttribArray(2);
}

void TerrainMesh::bufferData(std::vector<TerrainMesh::Vertex>& vertices, std::vector<unsigned int>& indices) {
	glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainMesh::Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

TerrainMesh::TerrainMesh(std::vector<TerrainMesh::Vertex>& verticies, std::vector<GLuint>& indicies)
	: Mesh{ indicies.size() } {
	init();
	bufferData(verticies, indicies);
}

TerrainMesh::~TerrainMesh() {}

