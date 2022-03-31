#include "MarchingCubes.h"
#include <assert.h>
#include <iomanip> // for printing prescision floats
#include <set>
#include <sstream>

unsigned int MarchingCubes::task_queue[static_cast<int>(MarchingCubes::RenderingStages::size)] = {}; // 0 initilize
unsigned int MarchingCubes::task_queue_max[static_cast<int>(MarchingCubes::RenderingStages::size)] = { UINT_MAX, UINT_MAX, UINT_MAX, 4, 4, 4, UINT_MAX };
// start, heightmap, scalar field, lighting, indicies, verticies, done
// 0      1          2             3         4          5         6
//unsigned int MarchingCubes::task_queue[7] = { 0, 0, 0, 0, 0, 0, 0 };
//unsigned int MarchingCubes::task_queue_max[7] = { UINT_MAX, UINT_MAX, UINT_MAX, 4, 4, 4, UINT_MAX };
// start, heightmap, scalar field, lighting, indicies, verticies, done
// 0      1          2             3         4         5          6

const int buffer = 2; // needs to be the same as buffer in ChunkManager.cpp and as int(2*overlap) in genHeightmap, and drawTexture

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

MarchingCubes::MarchingCubes(int cubeSize, glm::ivec3 pos, Heightmap& heightmapGenerator, ComputeShader& fillGenerator, ComputeShader& lightingCalculator, SSBOComputeShader& genIndices, SSBOComputeShader& genVerticies)
	: pos{ pos },
	verticies_on_side{ cubeSize },
	edges_on_side{ verticies_on_side - 1 },
	verticies_on_side_with_buffer{ verticies_on_side + buffer },

	heightmapGenerator{ heightmapGenerator },
	fillGenerator{ fillGenerator },
	lightingCalculator{ lightingCalculator },
	genEdges{ genIndices },
	genVerticies{ genVerticies }

{
	task_queue[static_cast<int>(current_step)] += 1;


	// Init SSBOs (output holds verticies, index holds number of verticies)
	glGenBuffers(1, &VERTEX_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, VERTEX_SSBO);
	VERTEX_SSBO_SIZE = SIZEOF_VERTEX * verticies_on_side * verticies_on_side * verticies_on_side * 12; // 12 edges per cube
	glBufferData(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_SIZE, NULL, GL_STATIC_COPY);
	//GLfloat clear_value = 0.0f;
	//glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_R32F, GL_RGBA32F, &clear_value);

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
		heightmapGenerator.releaseHeightmap(glm::ivec2(pos.x, pos.z));
	}

	if (!waiting) {
		task_queue[static_cast<int>(current_step)] -= 1;
	}
}

void MarchingCubes::update_cubes() {
	bool task_complete = fence.isDone();
	if (waiting) {
		if (task_queue_max[static_cast<int>(current_step)] == UINT_MAX ||
			task_queue[static_cast<int>(current_step)] < task_queue_max[static_cast<int>(current_step)]) {
			fence.set();
			task_queue[static_cast<int>(current_step)] += 1;
			waiting = false;

			switch (current_step) {
			case RenderingStages::start:
				// Should never happen since current_step starts at 0 and we just added 1
				break;
			case RenderingStages::genHeightmap:
				generate_heightmap();
				break;
			case RenderingStages::genField:
				generate_terrain_fills();
				break;
			case RenderingStages::genLighting:
				calculateLighting();
				break;
			case RenderingStages::genIndicies:
				heightmapGenerator.releaseHeightmap(glm::ivec2(pos.x, pos.z));
				HEIGHTMAP = 0;
				generate_indices();
				break;
				//			case 5:
			case RenderingStages::genVerticies:
			{
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
				GLuint data[5] = { 1, 1, 0, 0, 5 };
				glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, 5 * sizeof(GLuint), data);

				if (data[0] == 0) { // If there are no verticies to be generated
					fence.release();
					glDeleteTextures(1, &LANDSCAPE_DATA);
					glDeleteBuffers(1, &INDIRECT_SSBO);
					glDeleteBuffers(1, &EBO);
					glDeleteBuffers(1, &VERTEX_SSBO);
					glDeleteVertexArrays(1, &VAO);

					LANDSCAPE_DATA = 0;
					INDIRECT_SSBO = 0;
					EBO = 0;
					VERTEX_SSBO = 0;
					VAO = 0;

					task_queue[static_cast<int>(current_step)] -= 1;
					current_step = RenderingStages::empty;
				}
				else {
					generate_verticies();
				}
			}
			break;
			//			case 6:
			case RenderingStages::done:
				break;
			case RenderingStages::size:
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
			task_queue[static_cast<int>(current_step)] -= 1;
			++current_step;
			waiting = true;
			fence.release();
			update_cubes();
		}
		else {
			// wait for shader to finish
			return;
		}
	}

	// Be polite
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(GL_TEXTURE0 + 1);
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



void MarchingCubes::generate_heightmap() {
	glm::ivec2 coord = glm::ivec2(pos.x, pos.z);

	if (!heightmapGenerator.isGenerated(coord)) {
		heightmapGenerator.generateHeightmap(coord);
	}

	HEIGHTMAP = heightmapGenerator.getHeightmap(coord);
}

void MarchingCubes::generate_terrain_fills() {
	// Prepare render texture
	glActiveTexture(GL_TEXTURE0 + LANDSCAPE_DATA_UNIT);
	glGenTextures(1, &LANDSCAPE_DATA);
	glBindTexture(GL_TEXTURE_3D, LANDSCAPE_DATA);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, verticies_on_side_with_buffer, verticies_on_side_with_buffer, verticies_on_side_with_buffer, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(LANDSCAPE_DATA_UNIT, LANDSCAPE_DATA, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Heightmap
	glActiveTexture(GL_TEXTURE0 + HEIGHTMAP_UNIT);
	glBindTexture(GL_TEXTURE_2D, HEIGHTMAP);
	glBindImageTexture(HEIGHTMAP_UNIT, HEIGHTMAP, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	// Fill texture with render data
	// might be better to pass a pointer to the shader
	fillGenerator.use();
	fillGenerator.setVec3("offset", glm::vec3(pos));
	fillGenerator.fillTexture();
	fillGenerator.dontuse();
}

void MarchingCubes::calculateLighting() {
	// Scalar Field
	glActiveTexture(GL_TEXTURE0 + LANDSCAPE_DATA_UNIT);
	glBindTexture(GL_TEXTURE_3D, LANDSCAPE_DATA);
	glBindImageTexture(LANDSCAPE_DATA_UNIT, LANDSCAPE_DATA, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	lightingCalculator.use();
	lightingCalculator.fillTexture();
	lightingCalculator.dontuse();
}

void MarchingCubes::generate_indices() {
	gl_flush_errors();

	// Scalar Field
	glActiveTexture(GL_TEXTURE0 + LANDSCAPE_DATA_UNIT);
	glBindTexture(GL_TEXTURE_3D, LANDSCAPE_DATA);
	glBindImageTexture(LANDSCAPE_DATA_UNIT, LANDSCAPE_DATA, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

	// Indirect SSBO
	// Set initial count and indirect render information
	glGenBuffers(1, &INDIRECT_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INDIRECT_SSBO_BINDING, INDIRECT_SSBO);
	unsigned int data[5] = { 0, 1, 0, 0, 0 };

	glBufferData(GL_SHADER_STORAGE_BUFFER, 5 * sizeof(GLuint), data, GL_DYNAMIC_READ);

	// Vertex SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO_BINDING, VERTEX_SSBO);

	// Indicies
	// Generate data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, EBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, EBO_BINDING, EBO);
	EBO_SIZE = verticies_on_side * verticies_on_side * verticies_on_side * 15;
	glBufferData(GL_SHADER_STORAGE_BUFFER, (EBO_SIZE) * sizeof(GLuint), NULL, GL_STATIC_COPY);
	//GLfloat clear_value = 0.0f;
	//glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_R32F, GL_RGBA32F, &clear_value);

	gl_print_errors();

	genEdges.use();
	genEdges.setVec3("pos", glm::vec3(pos));
	//genEdges.setiVec3("chunk_size", glm::ivec3(verticies_on_side_with_buffer + 1)); // issue here. Index trick doesn't work like that
	genEdges.setiVec3("chunk_size", glm::ivec3(verticies_on_side + 1));
	const int fillsize = verticies_on_side; // buffer is intentionally ommitted
	genEdges.fillSSBO(EBO, EBO_BINDING, fillsize, fillsize, fillsize);
	genEdges.dontuse();
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
	genVerticies.use();
	genVerticies.setVec3("pos", glm::vec3(pos));
	//gen_verticies.setiVec3("chunk_size", glm::ivec3(verticies_on_side_with_buffer + 1)); // Issue also here
	genEdges.setiVec3("chunk_size", glm::ivec3(verticies_on_side + 1));
	genVerticies.fillSSBO(VERTEX_SSBO, VERTEX_SSBO_BINDING, VERTEX_SSBO_SIZE / SIZEOF_VERTEX, 1, 1);
	genVerticies.dontuse();
}

//////////////////////////////////////////////////////////////////////////////

void MarchingCubes::renderCubes(Shader* shader) {
	if (current_step == RenderingStages::empty) { return; }

	if (current_step == RenderingStages::done) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// Draw
		glBindVertexArray(VAO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, INDIRECT_SSBO);
		glBindBuffer(GL_ARRAY_BUFFER, VERTEX_SSBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		shader->use();
		glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
		shader->dontuse();

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}
	else if (current_step != RenderingStages::done) {
		update_cubes();
	}
};

void MarchingCubes::setPos(glm::vec3 p) {
	pos.x = (int)(p.x / verticies_on_side);
	pos.y = (int)(p.y / verticies_on_side);
	pos.z = (int)(p.z / verticies_on_side);
	pos *= verticies_on_side; // NOTE: This used to be (verticies_on_side + 1)
}

glm::vec3 MarchingCubes::getPos() {
	return pos;
}

void MarchingCubes::print_task() {
	return;

	//std::cout << current_step << " ";
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