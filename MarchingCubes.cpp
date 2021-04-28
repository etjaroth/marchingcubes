#include "MarchingCubes.h"
#include <assert.h>

MarchingCubes::MarchingCubes(int cubeSize, glm::ivec3 position, SSBOComputeShader* gen_verticies_ptr, ComputeShader* fill_generator_ptr) {
	gen_verticies = gen_verticies_ptr;
	fillGenerator = fill_generator_ptr;


	realCubeSize = cubeSize + 1;
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

	current_task = tasks::terrain_fills;
}


MarchingCubes::~MarchingCubes() {
	if (current_task != tasks::empty) {
		glDeleteBuffers(1, &INDIRECT_SSBO);
		glDeleteBuffers(1, &OUTPUT_SSBO);
		glDeleteVertexArrays(1, &VAO);
	}
}

void MarchingCubes::update_cubes() {
	glBindVertexArray(VAO);
	if (current_task == tasks::terrain_fills) { // create first fence
		generate_terrain_fills();
		++current_task;
	}
	else if (current_task != tasks::empty) {
		GLint syncStatus[1] = { GL_UNSIGNALED };
		glGetSynciv(fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
		bool finished = (syncStatus[0] == GL_SIGNALED);

		if (finished) {
			glDeleteSync(fence);
			++current_task; // do next task

			switch (current_task) {
			case tasks::buffer:
				++current_task;
				[[fallthrough]];
			case tasks::verticies:
				generate_verticies();
				break;
			case tasks::done: // check if empty
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
				unsigned int indirect_render_data[4] = { 0, 1, 0, 0 }; // count, instance_count, first, base_instance
				glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(unsigned int), indirect_render_data);
				if (indirect_render_data[0] == 0) {
					current_task = tasks::empty;
					// Do destructor
					glDeleteBuffers(1, &INDIRECT_SSBO);
					glDeleteBuffers(1, &OUTPUT_SSBO);
					glDeleteVertexArrays(1, &VAO);
				}
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

void MarchingCubes::generate_terrain_fills() {
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Prepare render texture
	glGenTextures(1, &landscape_data);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, landscape_data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, realCubeSize, realCubeSize, realCubeSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, landscape_data, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Fill texture with render data
	// might be better to pass a pointer to the shader
	fillGenerator->use();
	fillGenerator->setVec3("offset", glm::vec3(pos));
	fillGenerator->dontuse();
	fillGenerator->fillTexture(landscape_data);
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
	gen_verticies->fillSSBO(OUTPUT_SSBO, OUTPUT_SSBO_BINDING, realCubeSize-1, realCubeSize-1, realCubeSize-1);
	gen_verticies->dontuse();
	glDeleteTextures(1, &landscape_data); // might still need this?
}

void MarchingCubes::renderCubes(Shader* shader) {
	if (current_task == tasks::done || current_task == tasks::empty) {
		// Draw
		glBindVertexArray(VAO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
		glBindBuffer(GL_ARRAY_BUFFER, OUTPUT_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OUTPUT_SSBO_BINDING, OUTPUT_SSBO);

		shader->use();

		shader->setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
		glDrawArraysIndirect(GL_TRIANGLES, 0);

		shader->dontuse();
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}
	else {
		update_cubes();
	}
};

void MarchingCubes::setPos(glm::vec3 p) {
	pos.x = (int)(p.x / (realCubeSize - 1));
	pos.y = (int)(p.y / (realCubeSize - 1));
	pos.z = (int)(p.z / (realCubeSize - 1));
	pos *= realCubeSize;
}

glm::vec3 MarchingCubes::getPos() {
	return pos;
}