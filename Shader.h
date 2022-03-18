#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "Config.h"

class Shader
{
public:
    unsigned int shaderProgram;

    unsigned int vertexShader;
    unsigned int geometryShader;
    unsigned int fragmentShader;

    const char* vertexShaderSource;
    const char* geometryShaderSource;
    const char* fragmentShaderSource;

    void makeShader(const char* shaderPath, GLenum shaderType);

    //public:
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    ~Shader();
    void use() const; // Uses shader
    void dontuse() const;

    // Pass info to shaders
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};
#endif