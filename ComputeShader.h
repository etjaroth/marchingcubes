#pragma once

#include "Config.h"

class ComputeShader
{
private:
	unsigned int shader_program;
	unsigned int compute_shader;
	const char * shader_source;

    int tex_x, tex_y, tex_z;

public:
	ComputeShader(const char * filename, int x, int y, int z);
	void use() const;
	void dontuse() const;

    // 
    void fillTexture();
    GLuint generateTexture_3D(bool fill);
	void printInfo();

    void waitUntilDone();

    // Pass info to shaders
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setuInt(const std::string& name, unsigned int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setUvec2(const std::string& name, const glm::uvec2& value) const;
    void setUvec2(const std::string& name, unsigned int x, unsigned int y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};

