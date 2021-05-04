#include "MarchingCubes.h"
#include <assert.h>

MarchingCubes::MarchingCubes(int cubeSize, glm::ivec3 position, ComputeShader* heightmap_generator_ptr, ComputeShader* fill_generator_ptr, SSBOComputeShader* gen_verticies_ptr) {
	gen_verticies = gen_verticies_ptr;
	heightmap_generator = heightmap_generator_ptr;
	fillGenerator = fill_generator_ptr;

	cube_dimensions = cubeSize;
	vertex_cube_dimensions = cube_dimensions + 1;
	pos = position;

	//if (pos.y / cube_dimensions == 0)
	std::cout << "Loading: " << pos.x / cube_dimensions << ", " << pos.y / cube_dimensions << ", " << pos.z / cube_dimensions << std::endl;

	///////////////////////////////////////////////////////////////////////////

	// Init SSBOs (output holds verticies, index holds number of verticies)
	glGenBuffers(1, &INDIRECT_SSBO);
	glGenBuffers(1, &OUTPUT_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, OUTPUT_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OUTPUT_SSBO_BINDING, OUTPUT_SSBO);

	GLint ENOUGH_MEMORY_FOR_ALL_CASES = 4 * sizeof(int);
	ENOUGH_MEMORY_FOR_ALL_CASES *= (cubeSize + 1) * (cubeSize + 1) * (cubeSize + 1) * (5 * 3 * 4);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ENOUGH_MEMORY_FOR_ALL_CASES, NULL, GL_DYNAMIC_DRAW);

	// be polite
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OUTPUT_SSBO_BINDING, 0);


	// Set Vertex Attributes
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, OUTPUT_SSBO); // Just a reminder that we can do this

	// Vertex Position
	const unsigned int stride = 3 * 4 * sizeof(float);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	// Vertex Normal
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Vertex Material
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	current_task = tasks::start;
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}


MarchingCubes::~MarchingCubes() {
	if (current_task != tasks::empty) {
		glDeleteBuffers(1, &INDIRECT_SSBO);
		glDeleteBuffers(1, &OUTPUT_SSBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteSync(fence);
	}
}

void MarchingCubes::update_cubes() {
	glBindVertexArray(VAO);
	if (current_task != tasks::empty) {
		bool finished = true;
		if (current_task != tasks::start) {
			GLint syncStatus[1] = { GL_UNSIGNALED };
			glGetSynciv(fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
			finished = (syncStatus[0] == GL_SIGNALED);
		}

		if (finished) {
			glDeleteSync(fence); // fence is guarenteed to be set
			
			// do next task
			++current_task;
			print_task();
			switch (current_task) {
			case tasks::heightmap:
				generate_heightmap();
				break;
			case tasks::terrain_fills:
				generate_terrain_fills();
				break;
			case tasks::verticies:
				generate_verticies();
				break;
			case tasks::done: // check if empty
				//glBindBuffer(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO);
				//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
				//unsigned int indirect_render_data[4] = { 0, 1, 0, 0 }; // count, instance_count, first, base_instance
				//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(unsigned int), indirect_render_data);
				//if (indirect_render_data[0] == 0) {
				//	current_task = tasks::empty;
				//	// Do destructor
				//	glDeleteBuffers(1, &INDIRECT_SSBO);
				//	glDeleteBuffers(1, &OUTPUT_SSBO);
				//	glDeleteVertexArrays(1, &VAO);
				//}
				break;
			}
		}
	}

	// Be polite
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OUTPUT_SSBO_BINDING, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////////

void MarchingCubes::generate_heightmap() {
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Prepare render texture
	glGenTextures(1, &heightmap);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, heightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, vertex_cube_dimensions, vertex_cube_dimensions, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(1, heightmap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	heightmap_generator->use();
	heightmap_generator->setVec3("offset", glm::vec3(pos));
	heightmap_generator->fillTexture(heightmap);
	heightmap_generator->dontuse();
}

void MarchingCubes::generate_terrain_fills() {
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Prepare render texture
	glGenTextures(1, &landscape_data);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, landscape_data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, vertex_cube_dimensions, vertex_cube_dimensions, vertex_cube_dimensions, 0, GL_RGBA, GL_FLOAT, NULL);
	
	glBindImageTexture(0, landscape_data, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindImageTexture(1, heightmap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Fill texture with render data
	// might be better to pass a pointer to the shader
	fillGenerator->use();
	fillGenerator->setVec3("offset", glm::vec3(pos));
	fillGenerator->fillTexture(landscape_data);
	fillGenerator->dontuse();

	glDeleteTextures(1, &heightmap);
}

//void MarchingCubes::generate_edges() {}

void MarchingCubes::generate_verticies() {
	glBindTexture(GL_TEXTURE_3D, landscape_data);
	glBindImageTexture(0, landscape_data, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Set initial count and indirect render information
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
	unsigned int indirect_render_data[4] = { 0, 1, 0, 0 }; // count, instance_count, first, base_instance
	glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(unsigned int), indirect_render_data, GL_DYNAMIC_DRAW);

	// Generate vertex data
	gen_verticies->use();
	gen_verticies->setVec3("pos", glm::vec3(pos));
	gen_verticies->fillSSBO(OUTPUT_SSBO, OUTPUT_SSBO_BINDING, cube_dimensions, cube_dimensions, cube_dimensions);
	gen_verticies->dontuse();
	glDeleteTextures(1, &landscape_data);
}

//////////////////////////////////////////////////////////////////////////////

void MarchingCubes::renderCubes(Shader* shader) {
	if (current_task == tasks::done) {
		//std::cout << "=";
		// Draw
		glBindVertexArray(VAO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
		glBindBuffer(GL_ARRAY_BUFFER, OUTPUT_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OUTPUT_SSBO_BINDING, OUTPUT_SSBO);

		shader->use();
		glDrawArraysIndirect(GL_TRIANGLES, 0);
		shader->dontuse();

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}
	else if (current_task != tasks::empty) {
		update_cubes();
	}
};

void MarchingCubes::setPos(glm::vec3 p) {
	pos.x = (int)(p.x / cube_dimensions);
	pos.y = (int)(p.y / cube_dimensions);
	pos.z = (int)(p.z / cube_dimensions);
	pos *= vertex_cube_dimensions;
}

glm::vec3 MarchingCubes::getPos() {
	return pos;
}

void MarchingCubes::print_task() {
	if (current_task == tasks::start) {
		std::cout << "S";
	}
	else if (current_task == tasks::heightmap) {
		std::cout << "h";
	}
	else if (current_task == tasks::terrain_fills) {
		std::cout << "t";
	}
	else if (current_task == tasks::verticies) {
		std::cout << "v";
	}
	else if (current_task == tasks::done) {
		std::cout << "D";
	}
	else if (current_task == tasks::empty) {
		std::cout << "E";
	}
	else {
		std::cout << "?";
	}
}