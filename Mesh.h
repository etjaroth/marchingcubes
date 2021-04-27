#pragma once
#include "Config.h"
#include "Shader.h"

class Mesh
{
public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);// , std::vector<Texture> textures);
    void draw(Shader& shader, Material material);
private:
    //  render data
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};