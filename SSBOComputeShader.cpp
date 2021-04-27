#include "SSBOComputeShader.h"

SSBOComputeShader::SSBOComputeShader(const char* shader_file) {
	// Store Work Group Info


	// Compile Shader
	std::ifstream shaderFile;
	std::string line;
	std::string* sss;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Can now throw exceptions

	try {
		shaderFile.open(shader_file);
		std::stringstream rss;
		rss << shaderFile.rdbuf(); // Read from file
		shaderFile.close();

		sss = new std::string(rss.str()); // Have to convert through string become const char*

		shader_source = sss->c_str();
	}
	catch (std::ifstream::failure e)
	{
		shaderFile.close();
		std::cout << "ERROR::SHADER::COMPUTE::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		std::cout << e.what() << std::endl;
		exit(-1);
	}


	compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader, 1, &shader_source, NULL);

	// Compile Shder
	int shadercompilesuccess = 0; // Going to reuse these
	char infoLog[512];

	glCompileShader(compute_shader);
	glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &shadercompilesuccess); // Check for errors
	if (!shadercompilesuccess) // Uh oh
	{
		glGetShaderInfoLog(compute_shader, 512, NULL, infoLog);
		std::cout << "ERROR: COMPUTE SHADER (" << shader_file << ") COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(-1);
	}

	// Create and link the shader program
	shader_program = glCreateProgram(); // Stores program id
	glAttachShader(shader_program, compute_shader);
	glLinkProgram(shader_program); // Link

	int linksucess;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &linksucess); // Check for linking errors
	if (!linksucess) {
		glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
		std::cout << "ERROR: SSBO COMPUTE SHADER (" << shader_file << ") LINKING_FAILED\n" << infoLog << std::endl;
		return;
	}
	glDeleteShader(compute_shader); // Don't need this anymore
	glUseProgram(shader_program); // All shader calls will use shader_program
}

void SSBOComputeShader::use() const {
	glUseProgram(shader_program);
}

void SSBOComputeShader::dontuse() const {
	glBindVertexArray(0);
}

void SSBOComputeShader::fillSSBO(GLuint ssbo, unsigned int binding, GLuint x, GLuint y, GLuint z) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
	use();
	glDispatchCompute((GLuint)x, (GLuint)y, (GLuint)z); // fence or memory barrier is required
	dontuse();
};

// utility uniform functions
	// ------------------------------------------------------------------------
void SSBOComputeShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shader_program, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shader_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setuInt(const std::string& name, unsigned int value) const
{
	glUniform1ui(glGetUniformLocation(shader_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shader_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}
void SSBOComputeShader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(shader_program, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}
void SSBOComputeShader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shader_program, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}
void SSBOComputeShader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(shader_program, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void SSBOComputeShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}