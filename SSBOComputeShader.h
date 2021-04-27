#pragma once

#include "Config.h"

class SSBOComputeShader
{
    unsigned int shader_program;
    unsigned int compute_shader;
    const char* shader_source;

public:
    SSBOComputeShader(const char* filename);
    void use() const;
    void dontuse() const;

    void fillSSBO(GLuint ssbo, unsigned int binding, GLuint x, GLuint y, GLuint z);

    // Pass info to shaders
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setuInt(const std::string& name, unsigned int value) const;
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

