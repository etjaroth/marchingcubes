#pragma once
#include "Config.h"
#include "Mesh.h"

class TerrainMesh : public Mesh
{
public:
	void setupVAO() override;
	void bufferData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) override;

public:
	TerrainMesh(std::vector<Vertex>& verticies, std::vector<GLuint>& indicies);
	~TerrainMesh();
	TerrainMesh(TerrainMesh& other) = delete;
	TerrainMesh& operator=(TerrainMesh& other) = delete;
};

