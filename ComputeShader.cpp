#include "ComputeShader.h"

ComputeShader::ComputeShader(const char* shader_file, int x, int y, int z) {
	tex_x = x;
	tex_y = y;
	tex_z = z;
	
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
		std::cout << "ERROR: COMPUTE SHADER COMPILATION_FAILED\n" << infoLog << std::endl;
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
		std::cout << "ERROR: COMPUTE SHADER LINKING_FAILED\n" << infoLog << std::endl;
		return;
	}
	glDeleteShader(compute_shader); // Don't need this anymore
	glUseProgram(shader_program); // All shader calls will use shader_program
}

void ComputeShader::use() const {
	glUseProgram(shader_program);
}

void ComputeShader::dontuse() const {
	glBindVertexArray(0);
}

void ComputeShader::fillTexture(GLuint tex) {
	use();

	setVec3("boundryA", glm::vec3(10.0f, 0.0f, 1.0f));
	setVec3("boundryB", glm::vec3(100.0f, 0.0f, 1.0f));
	setuInt("biome_type_A", 0);
	setuInt("biome_type_B", 2);


	glDispatchCompute((GLuint)tex_x, (GLuint)tex_y, (GLuint)tex_z);

	// make sure writing to image has finished before read
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	dontuse();
};

GLuint ComputeShader::generateTexture_3D(bool fill) {
	GLuint tex_output;
	glGenTextures(1, &tex_output);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, tex_output);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, tex_x, tex_y, tex_z, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	if (fill) {
		fillTexture(tex_output);
	}
	return tex_output;
}

void ComputeShader::printInfo() {
	// check work group parameters
	int work_group_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_group_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_group_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_group_size[2]);
	printf("max global (total) work group counts x:%i y:%i z:%i\n",
		work_group_size[0], work_group_size[1], work_group_size[2]);

	int local_work_group_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &local_work_group_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &local_work_group_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &local_work_group_size[2]);
	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		local_work_group_size[0], local_work_group_size[1], local_work_group_size[2]);

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);
}

// utility uniform functions
	// ------------------------------------------------------------------------
void ComputeShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shader_program, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void ComputeShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shader_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void ComputeShader::setuInt(const std::string& name, unsigned int value) const
{
	glUniform1ui(glGetUniformLocation(shader_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void ComputeShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shader_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void ComputeShader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}
void ComputeShader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(shader_program, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void ComputeShader::setVec3(const std::string& name, const glm::vec3& value) const
{
	
	glUniform3fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}
void ComputeShader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shader_program, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void ComputeShader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}
void ComputeShader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(shader_program, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void ComputeShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ComputeShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ComputeShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}