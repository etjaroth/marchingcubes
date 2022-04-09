#include "Shader.h"

void checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void Shader::makeShader(const char* shaderPath, GLenum shaderType) {
	std::ifstream shaderFile;
	std::string line;
	std::string* sss;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Can now throw exceptions
	try {
		shaderFile.open(shaderPath);
		std::stringstream rss;
		rss << shaderFile.rdbuf(); // Read from file
		shaderFile.close();

		sss = new std::string(rss.str()); // Have to convert through string become const char*

		switch (shaderType) {
		case GL_VERTEX_SHADER:
			vertexShaderSource = sss->c_str();
			break;
		case GL_GEOMETRY_SHADER:
			geometryShaderSource = sss->c_str();
			break;
		case GL_FRAGMENT_SHADER:
			fragmentShaderSource = sss->c_str();
			break;
		};
	}
	catch (std::ifstream::failure e)
	{
		shaderFile.close();
		switch (shaderType) {
		case GL_VERTEX_SHADER:
			std::cout << "ERROR::SHADER::VERTEX::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			break;
		case GL_GEOMETRY_SHADER:
			std::cout << "ERROR::SHADER::GEOMETRY::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			break;
		case GL_FRAGMENT_SHADER:
			std::cout << "ERROR::SHADER::FRAGMENT::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			break;
		};
		std::cout << e.what() << std::endl;
		exit(-1);
	}

	// Compile shader
	switch (shaderType) {
	case GL_VERTEX_SHADER:
		vertexShader = glCreateShader(shaderType); // Prepare a shader
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Give the shader it's source code
		break;
	case GL_GEOMETRY_SHADER:
		geometryShader = glCreateShader(shaderType); // Prepare a shader
		glShaderSource(geometryShader, 1, &geometryShaderSource, NULL); // Give the shader it's source code
		break;
	case GL_FRAGMENT_SHADER:
		fragmentShader = glCreateShader(shaderType); // Prepare a shader
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Give the shader it's source code
		break;
	};

	// Compile Shder
	int shadercompilesuccess = 0; // Going to reuse these
	char infoLog[512];

	//loadShader(vertexPath, GL_VERTEX_SHADER, vertexShader, vertexShaderSource);
	// path, type, target, source

	switch (shaderType) {
	case GL_VERTEX_SHADER:
		glCompileShader(vertexShader); // Compile the shader
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shadercompilesuccess); // Check for errors
		break;
	case GL_GEOMETRY_SHADER:
		glCompileShader(geometryShader); // Compile the shader
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &shadercompilesuccess); // Check for errors
		break;
	case GL_FRAGMENT_SHADER:
		glCompileShader(fragmentShader); // Compile the shader
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shadercompilesuccess); // Check for errors
		break;
	};

	if (!shadercompilesuccess) // Uh oh
	{
		switch (shaderType) {
		case GL_VERTEX_SHADER:
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR: VERTEX SHADER COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		case GL_GEOMETRY_SHADER:
			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
			std::cout << "ERROR: GEOMETRY SHADER COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		case GL_FRAGMENT_SHADER:
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR: FRAGMENT SHADER COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		};
		exit(-1);
	}
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	makeShader(vertexPath, GL_VERTEX_SHADER);
	makeShader(fragmentPath, GL_FRAGMENT_SHADER);

	// Create and link the shader program
	shaderProgram = glCreateProgram(); // Stores program id
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); // Link

	int linksucess;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linksucess); // Check for linking errors
	if (!linksucess) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR: LINKING_FAILED\n" << infoLog << std::endl;
		return;
	}
	glDeleteShader(vertexShader); // Don't need this anymore
	glDeleteShader(fragmentShader); // Or this
	glUseProgram(shaderProgram); // All shader calls will use shaderProgram
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath) {
	///*
	makeShader(vertexPath, GL_VERTEX_SHADER);
	makeShader(geometryPath, GL_GEOMETRY_SHADER);
	makeShader(fragmentPath, GL_FRAGMENT_SHADER);

	// Create and link the shader program
	shaderProgram = glCreateProgram(); // Stores program id
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, geometryShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); // Link

	int linksucess;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linksucess); // Check for linking errors
	if (!linksucess) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR: LINKING_FAILED\n" << infoLog << std::endl;
		return;
	}
	glDeleteShader(vertexShader); // Don't need this anymore
	glDeleteShader(fragmentShader); // Or this
	glUseProgram(shaderProgram); // All shader calls will use shaderProgram


	/*/ 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex);
	glAttachShader(shaderProgram, fragment);
	if (geometryPath != nullptr)
		glAttachShader(shaderProgram, geometry);
	glLinkProgram(shaderProgram);
	checkCompileErrors(shaderProgram, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);*/

}

Shader::~Shader() {

}

void Shader::use() const {
	glUseProgram(shaderProgram);
}

void Shader::dontuse() const {
	glBindVertexArray(0);
}

// utility uniform functions
	// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
{
	const GLint g = glGetUniformLocation(shaderProgram, name.c_str());
	glUniform1f(g, value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}