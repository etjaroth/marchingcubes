#pragma once
#include "Config.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "SSBOComputeShader.h"
#include "Heightmap.h"

class MarchingCubes
{
private:
	glm::ivec3 pos;
	int cube_dimensions;
	int vertex_cube_dimensions;

	// Shaders
	Heightmap* heightmap_generator;
	ComputeShader* fillGenerator;
	SSBOComputeShader* gen_edges;
	SSBOComputeShader* gen_verticies;

	// rendering
	GLuint VAO;

	GLuint VERTEX_SSBO;
	const int VERTEX_SSBO_BINDING = 0;
	unsigned int VERTEX_SSBO_SIZE;
	const unsigned int SIZEOF_VERTEX = 4 * sizeof(float) * 3;

	GLuint EBO;
	const int EBO_BINDING = 1;

	GLuint INDIRECT_SSBO; // Holds the information needed for indirect rendering
	const int INDIRECT_SSBO_BINDING = 2;

	// Pipeline Textures (temporary)
	GLuint HEIGHTMAP = 0;
	const int HEIGHTMAP_UNIT = 1;

	GLuint LANDSCAPE_DATA = 0;
	const int LANDSCAPE_DATA_UNIT = 0;
	
	// Task                                              Shader Name         Task Number    Limit       Cost    Pipe                                                
	//                                                                                                                                              
	// OLD:                                                                                                                                         
	// start                                                                  0              Infinity    N/A     N/A                                
	// Generate heightmap                                genHeightmap.comp    1              Infinity    Low?    ETex2 -> Tex2                      
	// Generate scalar field                             drawTexture.comp     2              Infinity    Low?    ETex3, 1Tex2 -> Tex3               
	// wait                                                                   3              Infinity    N/A     N/A                                
	// Generate verticies                                genVerticies.comp    4              2           High    2Tex3 -> SSBO                      
	// done                                                                   5              Infinity    N/A     N/A                                
	//                                                                                                                                              
	// NEW:                                                                                                                                         
	// start                                                                  0              Infinity    N/A     N/A                                
	// Generate heightmap                                genHeightmap.comp    1              Infinity    Low?    ETex2 -> Tex2                      
	// Generate scalar field                             drawTexture.comp     2              Infinity    Low?    ETex3, 1Tex2 -> Tex3               
	// Mark verticies for generation, output indicies    genIndicies.comp     3              Infinity    Low?    ESSBO, ESSBO, 2Tex3 -> SSBO, SSBO  
	// Generate verticies                                genVerticies.comp    4              Infinity?   Low?    ESSBO, 3SSBO, 3SSBO -> 3SSBO, 3SSBO
	// done                                                                   5              Infinity    N/A     N/A                                
	//                                                                                                                                              
	// Pipe Legend:                                                                                                                                 
	// E...    empty (required to be created for this stage)                                                                                        
	// #...    ... is from task number #                                                                                                            
	// ...     is a Tex2, Tex3, or a SSBO                                                                                                           
	// Tex2    2d texture                                                                                                                           
	// Tex3    3D texture                                                                                                                           
	// SSBO    Shader Storeage Buffer Object                                                                                                        
	// Note: Input is listed in the same order as its stage's output                                                                                

	// Pipeline Variables
	unsigned int current_step = 0;
	bool waiting = false;
	static unsigned int task_queue[6];
	static unsigned int task_queue_max[6];
		// Fence
	GLsync fence;
	bool fence_is_active = false;

	// Render Pipeline Functions
	void update_cubes();
	void generate_heightmap();
	void generate_terrain_fills();
	void generate_indices();
	void generate_verticies();
		// Fence
	bool fence_is_done();
	void set_fence();
	void free_fence();

	void print_task();


public:
	MarchingCubes(int cubeSize, glm::ivec3 position, Heightmap* heightmap_generator_ptr, ComputeShader* fill_generator_ptr, SSBOComputeShader* gen_indices_ptr, SSBOComputeShader* gen_verticies_ptr);
	~MarchingCubes();
	void renderCubes(Shader* shader);
	void setPos(glm::vec3 p);
	glm::vec3 getPos();
};