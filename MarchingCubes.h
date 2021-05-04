#pragma once
#include "Config.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"




class MarchingCubes
{
public:
	glm::ivec3 pos;
	int cube_dimensions;
	int vertex_cube_dimensions;

	// Shaders
	ComputeShader* heightmap_generator;
	ComputeShader* fillGenerator;
	SSBOComputeShader* gen_verticies;

	// rendering
	unsigned int  VAO;
	unsigned int OUTPUT_SSBO;
	const int OUTPUT_SSBO_BINDING = 0;
	unsigned int INDIRECT_SSBO;
	const int INDIRECT_SSBO_BINDING = 1;

	// Pipeline Textures
	GLuint heightmap = 0;
	GLuint landscape_data = 0;
	
	// tasks
	enum class tasks {start=0, heightmap, terrain_fills, verticies, done, empty};
	friend tasks& operator++(tasks& orig)
	{
		if (orig == tasks::done) {
			return orig;
		}
		orig = static_cast<tasks>((int)orig + 1);
		return orig;
	};

	friend tasks operator++(tasks& orig, int)
	{
		tasks rVal = orig;
		++orig;
		return rVal;
	}

	tasks current_task = tasks::start;
	GLsync fence;

	void update_cubes();
	void generate_heightmap();
	void generate_terrain_fills();
	//void generate_edges();
	void generate_verticies();

	void print_task();

public:
	MarchingCubes(int cubeSize, glm::ivec3 position, ComputeShader* heightmap_generator_ptr, ComputeShader* fill_generator_ptr, SSBOComputeShader* gen_verticies_ptr);
	~MarchingCubes();
	void renderCubes(Shader* shader);
	void setPos(glm::vec3 p);
	glm::vec3 getPos();
};