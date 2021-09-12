#pragma once
#include "Config.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"



class MarchingCubes
{
public:
	glm::ivec3 pos;
	int cube_dimensions;
	int vertex_cube_dimensions;

	// Shaders
	Heightmap* heightmap_generator;
	ComputeShader* fillGenerator;
	SSBOComputeShader* gen_edges;
	SSBOComputeShader* gen_verticies;

	// rendering
	unsigned int VAO;
	unsigned int OUTPUT_SSBO;
	const int OUTPUT_SSBO_BINDING = 0;
	unsigned int INDIRECT_SSBO;
	const int INDIRECT_SSBO_BINDING = 1;

	// Pipeline Textures
	GLuint heightmap = 0;
	GLuint edge_data = 0;
	unsigned int edge_data_binding = 2;
	GLuint landscape_data = 0;
	
	// Task                             Shader Name          Task Number    Limit       Cost    Pipe
	// Generate heightmap               genHeightmap.comp    1              Infinity    Low?    ETex2 -> Tex2
	// Generate marching cube voxels    drawTexture.comp     2              Infinity    Low?    ETex3, 0Tex2 -> Tex3
	// Generate verticies               genVerticies.comp    4              2           High    2Tex3 -> SSBO
	// 
	// Pipe Legend:
	// E...    empty (required to be created for this stage)
	// #...    ... is from task number #
	// ...     is a Tex2, Tex3, or a SSBO
	// Tex2    2d texture
	// Tex3    3D texture
	// SSBO    Shader Storeage Buffer Object
	// Note: Input is listed in the same order as its stage's output

	enum class tasks {start=0, heightmap, terrain_fills, waiting, verticies, done, empty};
	static int max_stage_count; // limits the amount of verticies stages at any one time. Static so that the max number of stages can be changed in a menu
	static int stage_count;
	bool assigned_stage = false;

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
	bool fence_is_active = false;

	void update_cubes();
	void generate_heightmap();
	void generate_terrain_fills();
	void generate_edges();
	void generate_verticies();

	bool fence_is_done();
	void set_fence();
	void free_fence();

	void print_task();


public:
	MarchingCubes(int cubeSize, glm::ivec3 position, Heightmap* heightmap_generator_ptr, ComputeShader* fill_generator_ptr, SSBOComputeShader* gen_verticies_ptr);
	~MarchingCubes();
	void renderCubes(Shader* shader);
	void setPos(glm::vec3 p);
	glm::vec3 getPos();
};