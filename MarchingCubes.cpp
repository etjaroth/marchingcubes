#include "MarchingCubes.h"
#include <assert.h>

int MarchingCubes::max_stage_count = 2;
int MarchingCubes::stage_count = 0;

MarchingCubes::MarchingCubes(int cubeSize, glm::ivec3 position, Heightmap* heightmap_generator_ptr, ComputeShader* fill_generator_ptr, SSBOComputeShader* gen_verticies_ptr) {
	gen_verticies = gen_verticies_ptr;
	heightmap_generator = heightmap_generator_ptr;
	fillGenerator = fill_generator_ptr;

	cube_dimensions = cubeSize;
	vertex_cube_dimensions = cube_dimensions + 1;
	pos = position;

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
	glBindBuffer(GL_ARRAY_BUFFER, OUTPUT_SSBO);

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
	set_fence();
}


MarchingCubes::~MarchingCubes() {
	if (current_task != tasks::empty) {
		heightmap_generator->release_heightmap(glm::ivec2(pos.x, pos.z));
		glDeleteBuffers(1, &INDIRECT_SSBO);
		glDeleteBuffers(1, &OUTPUT_SSBO);
		//glDeleteVertexArrays(1, &VAO);
	}
	if (assigned_stage) {
		--stage_count;
	}
	free_fence();
}

void MarchingCubes::update_cubes() {
	glBindVertexArray(VAO);
	if (current_task != tasks::empty) {
		bool finished = true;
		if (current_task != tasks::start) {
			finished = fence_is_done();
		}
		if (current_task == tasks::waiting) {
			if (stage_count < max_stage_count) {
				++stage_count;
				assigned_stage = true;
			}
			finished = assigned_stage;
		}

		if (finished) {
			free_fence();

			// do next task
			++current_task;

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
				--stage_count;
				assigned_stage = false;
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
	//fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	glm::ivec2 coord = glm::ivec2(pos.x, pos.z);

	if (heightmap_generator->is_generated(coord)) {
		//glDeleteSync(fence);
	}
	else {
		heightmap_generator->generate_heightmap(coord);
	}

	heightmap = heightmap_generator->get_heightmap(coord);
}

void MarchingCubes::generate_terrain_fills() {
	set_fence();

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

	//glDeleteTextures(1, &heightmap);
}

void MarchingCubes::generate_edges() {
	glBindTexture(GL_TEXTURE_3D, landscape_data);
	glBindImageTexture(0, landscape_data, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	set_fence();

	// Create SSBO (edge_data)
	glGenBuffers(1, &edge_data);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edge_data);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, edge_data_binding, edge_data);

	GLint ENOUGH_MEMORY_FOR_ALL_CASES = 4 * sizeof(int);
	ENOUGH_MEMORY_FOR_ALL_CASES *= vertex_cube_dimensions * vertex_cube_dimensions * vertex_cube_dimensions * (3 * 4); // verticies are 3 vec4s (12 bytes)
	glBufferData(GL_SHADER_STORAGE_BUFFER, ENOUGH_MEMORY_FOR_ALL_CASES, NULL, GL_DYNAMIC_DRAW);

	// Set initial count and indirect render information
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edge_data);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, edge_data_binding, edge_data);
	unsigned int render_data_init[4] = { 0, 1, 0, 0 }; // count, instance_count, first, base_instance
	glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(unsigned int), render_data_init, GL_DYNAMIC_DRAW);

	// Generate vertex data
	gen_edges->use();
	gen_edges->setVec3("pos", glm::vec3(pos));
	gen_edges->fillSSBO(edge_data, edge_data_binding, cube_dimensions, cube_dimensions, cube_dimensions);
	gen_edges->dontuse();
}

void MarchingCubes::generate_verticies() {
	glBindTexture(GL_TEXTURE_3D, landscape_data);
	glBindImageTexture(0, landscape_data, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edge_data);
	//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, edge_data_binding, edge_data);
	set_fence();

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
	print_task();
	if (current_task == tasks::done) {
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

bool MarchingCubes::fence_is_done() {
	GLint syncStatus[1] = { GL_UNSIGNALED };
	glGetSynciv(fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
	return (syncStatus[0] == GL_SIGNALED);
}

void MarchingCubes::set_fence() {
	if (!fence_is_active) {
		fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}
}

void MarchingCubes::free_fence() {
	if (fence_is_active) {
		glDeleteSync(fence);
	}
}


void MarchingCubes::print_task() {
	return;


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