#include "Shader.h"

/***********************************************************************************/
Shader::Shader(const std::string& VertexShader, const std::string& PixelShader) {
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::stringstream vShaderStream, fShaderStream;
	
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		// Open files
		vShaderFile.open(VertexShader);
		fShaderFile.open(PixelShader);
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cerr << "Shader error: " << e.what();
		FILE_LOG(logERROR) << "Shader error " << e.what();
	}
	catch (...) {
		std::cout << "Unhandled exception in shader class.\n";
	}

	prepareShader(vShaderStream.str().c_str(), fShaderStream.str().c_str());
}

/***********************************************************************************/
Shader::~Shader() {
}

/***********************************************************************************/
void Shader::Use() {
	glUseProgram(m_shaderProgram);
}

/***********************************************************************************/
GLint Shader::GetUniformLoc(const std::string& Uniform) const {
	GLint loc = glGetUniformLocation(m_shaderProgram, Uniform.c_str());
	if (loc == -1) {
		std::string error = "Uniform: " + Uniform + " does not exist.\n";

		std::cerr << error;
		FILE_LOG(logERROR) << error;
	}
	return loc;
}

/***********************************************************************************/
void Shader::prepareShader(const GLchar* vertexShader, const GLchar* pixelShader) {

	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];

	// Vertex shader
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertexShader, 1, &vertexShader, NULL);
	glCompileShader(m_vertexShader);
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(m_vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR: vertex shader compilation failed:\n" << infoLog << std::endl;
		FILE_LOG(logERROR) << infoLog;
	}

	// Fragment shader
	m_pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_pixelShader, 1, &pixelShader, NULL);
	glCompileShader(m_pixelShader);
	// Check for compile time errors
	glGetShaderiv(m_pixelShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(m_pixelShader, 512, NULL, infoLog);
		std::cerr << "ERROR: fragment shader compilation failed:\n" << infoLog << std::endl;
		FILE_LOG(logERROR) << infoLog;
	}

	// Link shaders
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, m_vertexShader);
	glAttachShader(m_shaderProgram, m_pixelShader);
	glLinkProgram(m_shaderProgram);
	// Check for linking errors
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR: shaders failed to link:\n" << infoLog << std::endl;
		FILE_LOG(logERROR) << infoLog;
	}

	std::cout << "Comiled: " << m_vertexShader << '\n';
	//Clean up
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_pixelShader);
}
