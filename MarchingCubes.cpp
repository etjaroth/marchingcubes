#include "MarchingCubes.h"
#include <assert.h>
#include <iomanip> // for printing prescision floats

unsigned int MarchingCubes::task_queue[6] = { 0, 0, 0, 0, 0, 0 };
unsigned int MarchingCubes::task_queue_max[6] = { UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX };
// start, heightmap, scalar field, indicies, verticies, done
// 0      1          2             3         4          5

std::string glGetErrorString(GLenum err)
{
	switch (err)
	{
		// opengl 2 errors (8)
	case GL_NO_ERROR:
		return "GL_NO_ERROR";

	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";

	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";

	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";

	case GL_STACK_OVERFLOW:
		return "GL_STACK_OVERFLOW";

	case GL_STACK_UNDERFLOW:
		return "GL_STACK_UNDERFLOW";

	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";

		// opengl 3 errors (1)
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";

		// gles 2, 3 and gl 4 error are handled by the switch above
	default:
		return "Unknown_Error " + std::to_string((unsigned int)err);
	}
}

void gl_flush_errors() {
	while (glGetError() != GL_NO_ERROR) {}
};

void gl_print_errors() {
	return;

	std::cout << "GL_Errors:" << std::endl;
	while (true) {
		GLenum err = glGetError();
		if (err == GL_NO_ERROR) {
			break;
		}
		else {
			std::cout << "    " << glGetErrorString(err) << std::endl;
		}
	}
}

MarchingCubes::MarchingCubes(int cubeSize, glm::ivec3 position, Heightmap* heightmap_generator_ptr, ComputeShader* fill_generator_ptr, SSBOComputeShader* gen_indices_ptr, SSBOComputeShader* gen_verticies_ptr) {
	task_queue[current_step] += 1;

	heightmap_generator = heightmap_generator_ptr;
	fillGenerator = fill_generator_ptr;
	gen_edges = gen_indices_ptr;
	gen_verticies = gen_verticies_ptr;

	verticies_on_side = cubeSize;
	edges_on_side = verticies_on_side - 1;
	verticies_on_side_with_buffer = verticies_on_side + 2;

	pos = position;

	///////////////////////////////////////////////////////////////////////////

	// Init SSBOs (output holds verticies, index holds number of verticies)
	glGenBuffers(1, &VERTEX_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, VERTEX_SSBO);
	VERTEX_SSBO_SIZE = SIZEOF_VERTEX * verticies_on_side * verticies_on_side * verticies_on_side * 12; // 12 edges per cube
	glBufferData(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_SIZE, NULL, GL_STATIC_DRAW);

	GLint size = 0;
	glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);

		// be polite
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, 0);

	// Set Vertex Attributes
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VERTEX_SSBO);

	// Vertex Position
	const unsigned int stride = 3 * 4 * sizeof(GLfloat);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	// Vertex Normal
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Vertex Material
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


MarchingCubes::~MarchingCubes() {
	glDeleteTextures(1, &LANDSCAPE_DATA);
	glDeleteBuffers(1, &INDIRECT_SSBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VERTEX_SSBO);
	glDeleteVertexArrays(1, &VAO);
	if (HEIGHTMAP != 0) {
		heightmap_generator->release_heightmap(glm::ivec2(pos.x, pos.z));
	}

	if (!waiting) {
		task_queue[current_step] -= 1;
	}

	free_fence();
}

void MarchingCubes::update_cubes() {
	bool task_complete = fence_is_done();
	if (waiting) {
		if (task_queue[current_step] < task_queue_max[current_step]) {
			set_fence();
			task_queue[current_step] += 1;
			waiting = false;

			switch (current_step) {
			case 0:
				// Should never happen since current_step starts at 0 and we just added 1
				break;
			case 1:
				generate_heightmap();
				break;
			case 2:
				generate_terrain_fills();
				break;
			case 3:
				heightmap_generator->release_heightmap(glm::ivec2(pos.x, pos.z));
				HEIGHTMAP = 0;
				generate_indices();
				break;
			case 4:
			{
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
				GLuint data[5] = { 0, 1, 0, 0, 5 };
				glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, 5 * sizeof(GLuint), data);

				if (data[0] == 0) { // If there are no verticies to be generated
					free_fence();
					glDeleteTextures(1, &LANDSCAPE_DATA);
					glDeleteBuffers(1, &INDIRECT_SSBO);
					glDeleteBuffers(1, &EBO);
					glDeleteBuffers(1, &VERTEX_SSBO);
					glDeleteVertexArrays(1, &VAO);

					INDIRECT_SSBO = 0;
					EBO = 0;
					VERTEX_SSBO = 0;
					VAO = 0;

					task_queue[current_step] -= 1;
					current_step = 6;

				}
				else {
					generate_verticies();
				}
			}
			break;
			case 5:
				//glDeleteTextures(1, &LANDSCAPE_DATA);
				break;
			case 6:
				std::cout << "Something went wrong" << std::endl;
				break;
			}
		}
		else {
			// wait for empty slot
			return;
		}
	}
	else {
		if (task_complete) {
			task_queue[current_step] -= 1;
			++current_step;
			waiting = true;
			free_fence();
			update_cubes();
		}
		else {
			// wait for shader to finish
			return;
		}
	}

	// Be polite
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////////

const int buffer = 2;

void MarchingCubes::generate_heightmap() {
	glm::ivec2 coord = glm::ivec2(pos.x, pos.z);

	if (!heightmap_generator->is_generated(coord)) {
		heightmap_generator->generate_heightmap(coord);
	}

	HEIGHTMAP = heightmap_generator->get_heightmap(coord);
}

void MarchingCubes::generate_terrain_fills() {
	// Prepare render texture
	glGenTextures(1, &LANDSCAPE_DATA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, LANDSCAPE_DATA);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	const int verticies_on_side_with_buffer = verticies_on_side + 2;
	//const int verticies_on_side_with_buffer = verticies_on_side;
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, verticies_on_side_with_buffer, verticies_on_side_with_buffer, verticies_on_side_with_buffer, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, LANDSCAPE_DATA, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Heightmap
	glActiveTexture(GL_TEXTURE0 + HEIGHTMAP_UNIT);
	glBindImageTexture(HEIGHTMAP_UNIT, HEIGHTMAP, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Fill texture with render data
	// might be better to pass a pointer to the shader
	fillGenerator->use();
	fillGenerator->setVec3("offset", glm::vec3(pos));
	fillGenerator->fillTexture(LANDSCAPE_DATA);
	fillGenerator->dontuse();
}

void MarchingCubes::generate_indices() {
	// Scalar Field
	glActiveTexture(GL_TEXTURE0 + LANDSCAPE_DATA_UNIT);
	glBindTexture(GL_TEXTURE_3D, LANDSCAPE_DATA);
	glBindImageTexture(LANDSCAPE_DATA_UNIT, LANDSCAPE_DATA, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Indirect SSBO
	// Set initial count and indirect render information
	glGenBuffers(1, &INDIRECT_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
	unsigned int data[5] = { 0, 1, 0, 0, 0 };
	glBufferData(GL_SHADER_STORAGE_BUFFER, 5 * sizeof(GLuint), data, GL_STATIC_DRAW);

	// Vertex SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, VERTEX_SSBO);

	// Indicies
	// Generate data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, EBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, EBO_BINDING, EBO);
	EBO_SIZE = verticies_on_side * verticies_on_side * verticies_on_side * 15;
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		(EBO_SIZE) * sizeof(GLuint),
		NULL, GL_STATIC_DRAW);

	gen_edges->use();
	gen_edges->setVec3("pos", glm::vec3(pos));
	gen_edges->setiVec3("chunk_size", glm::ivec3(verticies_on_side + 2));
	const int fillsize = verticies_on_side;
	gen_edges->fillSSBO(EBO, EBO_BINDING, fillsize, fillsize, fillsize);
	gen_edges->dontuse();
}

void MarchingCubes::generate_verticies() {
	// Scalar field
	glActiveTexture(GL_TEXTURE0 + LANDSCAPE_DATA_UNIT);
	glBindTexture(GL_TEXTURE_3D, LANDSCAPE_DATA);
	glBindImageTexture(LANDSCAPE_DATA_UNIT, LANDSCAPE_DATA, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// EBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, EBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, EBO_BINDING, EBO);

	// Vertex SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, VERTEX_SSBO);

	// Generate vertex data
	gen_verticies->use();
	gen_verticies->setVec3("pos", glm::vec3(pos));
	gen_verticies->setiVec3("chunk_size", glm::ivec3(verticies_on_side + 2));
	gen_verticies->fillSSBO(VERTEX_SSBO, VERTEX_SSBO_BINDING, VERTEX_SSBO_SIZE / SIZEOF_VERTEX, 1, 1);
	gen_verticies->dontuse();
}

//////////////////////////////////////////////////////////////////////////////

void MarchingCubes::renderCubes(Shader* shader) {
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	gl_print_errors();

	if (current_step == 5) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// Draw


		glBindVertexArray(VAO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
		glBindBuffer(GL_ARRAY_BUFFER, VERTEX_SSBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// For testing:
		// 0 draws triangles only (normal)
		// 1 draws verticies only
		// 2 draws both
		const unsigned int render_mode = 0;
		unsigned int data[5] = { edges_on_side * edges_on_side * edges_on_side * edges_on_side, 0, 0, 0, 0 };
		switch (render_mode) {
		case 0:
		{
			shader->use();
			glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
			shader->dontuse();
			break;
		}
		case 1:
			glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(GLuint), data);
			shader->use();
			glPointSize(10.0f);
			glDrawArraysIndirect(GL_POINTS, 0);
			shader->dontuse();
			break;
		case 2:
			shader->use();
			glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);


			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
			glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(GLuint), data);


			glPointSize(10.0f);
			glDrawArraysIndirect(GL_POINTS, 0);
			shader->dontuse();
			break;
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}
	else if (current_step != 6) {
		update_cubes();
	}
};

void MarchingCubes::setPos(glm::vec3 p) {
	pos.x = (int)(p.x / verticies_on_side);
	pos.y = (int)(p.y / verticies_on_side);
	pos.z = (int)(p.z / verticies_on_side);
	pos *= verticies_on_side - 1; // NOTE: This used to be (verticies_on_side + 1)
}

glm::vec3 MarchingCubes::getPos() {
	return pos;
}

bool MarchingCubes::fence_is_done() {
	if (!fence_is_active) {
		return true;
	}

	GLint syncStatus[1] = { GL_UNSIGNALED };
	glGetSynciv(fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
	return (syncStatus[0] == GL_SIGNALED);
}

void MarchingCubes::set_fence() {
	if (!fence_is_active) {
		fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		fence_is_active = true;
	}
}

void MarchingCubes::free_fence() {
	if (fence_is_active) {
		glDeleteSync(fence);
		fence_is_active = false;
	}
}


void MarchingCubes::print_task() {
	//return;

	std::cout << current_step << " ";
	//return;
	//switch (current_step) {
	//case 0:
	//	std::cout << "S";
	//	break;
	//case 1:
	//	std::cout << "h";
	//	break;
	//case 2:
	//	break;
	//case 3:
	//	break;
	//case 4:
	//	break;
	//case 5:
	//	break;
	//case 6:
	//	break;
	//}
}