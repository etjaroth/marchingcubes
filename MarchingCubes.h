#pragma once
#include "Config.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"




class MarchingCubes
{
public:
	glm::ivec3 pos;

	// Shaders
	int realCubeSize;
	//FillCache noise_cache;
	SSBOComputeShader *gen_verticies;
	ComputeShader* fillGenerator;

	// rendering
	unsigned int  VAO;
	unsigned int OUTPUT_SSBO;
	const int OUTPUT_SSBO_BINDING = 0;
	unsigned int INDIRECT_SSBO;
	const int INDIRECT_SSBO_BINDING = 1;

	unsigned int* ssbo_index_ptr;

	// generating terrain
	
	// tasks
	enum class tasks {terrain_fills=0, buffer, verticies, done, empty};
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

	tasks current_task;
	GLsync fence;

	GLuint landscape_data;
	const char* texture_shader_file_name;

	void update_cubes();
	void generate_terrain_fills();
	//void generate_edges();
	void generate_verticies();

public:
	MarchingCubes(int cubeSize, glm::ivec3 position, SSBOComputeShader* gen_verticies_ptr, ComputeShader* fill_generator_ptr);
	~MarchingCubes();
	void renderCubes(Shader* shader);
	void setPos(glm::vec3 p);
};