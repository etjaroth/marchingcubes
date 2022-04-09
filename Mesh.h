#pragma once
#include "Config.h"
#include "Shader.h"

class Mesh
{
public:
    struct Vertex {
        glm::vec4 pos;
        glm::vec4 normal;
        glm::vec4 material;
    };

    GLuint size = 0;

public:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    virtual void setupVAO() = 0;
    virtual void bufferData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) = 0;

public:
    explicit Mesh(unsigned int size);
    virtual ~Mesh();
    void render(Shader& shader);
    void init();
};