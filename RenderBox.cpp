#include "RenderBox.h"

RenderBox::RenderBox() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texturePos));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

RenderBox::~RenderBox() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void RenderBox::render(Texture2D& texture, FPSCamera& camera, double time) const {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	
	shader.use();
	texture.use(0);
	shader.setFloat("time", time);

	glm::vec3 rotationVec = camera.getCameraRotationVec();

	glm::mat4 m(1.0f);
	m = glm::rotate(m, rotationVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::rotate(m, rotationVec.y, glm::vec3(0.0f, 1.0f, 0.0f));
	m = glm::rotate(m, rotationVec.z, glm::vec3(0.0f, 0.0f, 1.0f));

	m = glm::translate(m, glm::vec3(0.0f, 0.26, 0.0f)); // 0.26 is from #define ELEVATION_CONSTANT 0.26
	shader.setMat4("viewMatInverse", glm::inverse(m));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	texture.dontuse();
	shader.dontuse();
}

void RenderBox::render(GLuint tex, FPSCamera& camera, double time) const {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	shader.use();
	shader.setFloat("time", time);

	glm::vec3 rotationVec = camera.getCameraRotationVec();

	glm::mat4 m(1.0f);
	m = glm::rotate(m, rotationVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::rotate(m, rotationVec.y, glm::vec3(0.0f, 1.0f, 0.0f));
	m = glm::rotate(m, rotationVec.z, glm::vec3(0.0f, 0.0f, 1.0f));

	m = glm::translate(m, glm::vec3(0.0f, 0.26, 0.0f)); // 0.26 is from #define ELEVATION_CONSTANT 0.26
	shader.setMat4("viewMatInverse", glm::inverse(m));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	shader.dontuse();
}