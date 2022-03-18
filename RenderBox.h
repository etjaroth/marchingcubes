#pragma once
#include "Config.h"
#include "shader.h"
#include "texture.h"

class RenderBox
{
	struct Vertex {
		glm::vec3 pos;
		glm::vec2 texturePos;
	};

	Vertex vertices[4] = {
		// positions           // texture coords
		{glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec2(1.0f, 1.0f)}, // top right
		{glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)}, // bottom right
		{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)}, // bottom left
		{glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f)}  // top left 
	};

	const unsigned int indices[6] = {
		3, 1, 0, // first triangle
		3, 2, 1  // second triangle
	};


	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	Shader shader{ "RenderBoxVertexShader.vert", "RenderBoxFragmentShader.frag" };

public:
	RenderBox();
	~RenderBox();

	void render(Texture2D& texture) const;
	void render(GLuint tex) const;
};

