#include "Mesh.h"

Mesh::Mesh(unsigned int size)
	: size{ size - (size % 3) } {
	// remember to call init() in child class
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	VAO = 0;
	VBO = 0;
	EBO = 0;
}

void Mesh::render(Shader& shader) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	shader.use();
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
	shader.dontuse();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::init() {
	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	setupVAO();

	// EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}