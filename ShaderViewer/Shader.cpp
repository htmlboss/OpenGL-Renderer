#include "Shader.h"

#include <log.h>
#include <fstream>
#include <sstream>

/***********************************************************************************/
Shader::Shader(const std::string& VertexShader, const std::string& PixelShader) {
	
	// Retrieve the vertex/fragment source code from filePath
	auto vertexCode = readShader(VertexShader);
	auto fragmentCode = readShader(PixelShader);

	// Compile and link shaders
	prepareShader(vertexCode.c_str(), fragmentCode.c_str());
}

/***********************************************************************************/
Shader::Shader(const std::string& VertexShader, const std::string& PixelShader, const std::string& GeometryShader) {
	// Retrieve the vertex/fragment source code from filePath
	auto vertexCode = readShader(VertexShader);
	auto fragmentCode = readShader(PixelShader);
	auto geometryCode = readShader(GeometryShader);

	// Compile and link shaders
	prepareShader(vertexCode.c_str(), fragmentCode.c_str(), geometryCode.c_str());
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
std::string Shader::readShader(const std::string& ShaderPath) {

	std::string shaderCode;
	std::ifstream file;
	std::stringstream fileSS;

	file.exceptions(std::ifstream::badbit);
	try {
		// Open files
		file.open(ShaderPath);
		// Read file's buffer contents into streams
		fileSS << file.rdbuf();
		// close file handlers
		file.close();
		// Convert stream into string
		shaderCode = fileSS.str();
	}
	catch (std::ifstream::failure& e) {
		std::cerr << "Shader error: " << e.what();
		FILE_LOG(logERROR) << "Shader error " << e.what();
	}
	catch (...) {
		std::cout << "Unhandled exception in Shader::readShader().\n";
	}

	return shaderCode;
}

/***********************************************************************************/
void Shader::prepareShader(const GLchar* vertexShader, const GLchar* pixelShader) {

	// Vertex shader
	compileShader(vertexShader, ShaderType::VertexShader, m_vertexShader);

	// Fragment shader
	compileShader(pixelShader, ShaderType::PixelShader, m_pixelShader);

	// Link shaders
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, m_vertexShader);
	glAttachShader(m_shaderProgram, m_pixelShader);
	glLinkProgram(m_shaderProgram);
	// Check for linking errors
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, m_infoLog);
		std::cerr << "ERROR: shaders failed to link:\n" << m_infoLog << std::endl;
		FILE_LOG(logERROR) << m_infoLog;
	}

	std::cout << "Compiled: " << m_shaderProgram << '\n';
	// Clean up
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_pixelShader);
}

/***********************************************************************************/
void Shader::prepareShader(const GLchar* vertexShader, const GLchar* pixelShader, const GLchar* geometryShader) {

	// Vertex shader
	compileShader(vertexShader, ShaderType::VertexShader, m_vertexShader);

	// Fragment shader
	compileShader(pixelShader, ShaderType::PixelShader, m_pixelShader);

	// Geometry Shader
	compileShader(geometryShader, ShaderType::GeometryShader, m_geometryShader);

	// Link
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, m_vertexShader);
	glAttachShader(m_shaderProgram, m_geometryShader);
	glAttachShader(m_shaderProgram, m_pixelShader);
	// Check for linking errors
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, m_infoLog);
		std::cerr << "ERROR: shaders failed to link:\n" << m_infoLog << std::endl;
		FILE_LOG(logERROR) << m_infoLog;
	}

	std::cout << "Compiled: " << m_shaderProgram << '\n';
	//Clean up
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_geometryShader);
	glDeleteShader(m_pixelShader);
}

/***********************************************************************************/
void Shader::compileShader(const GLchar* shaderSource, const ShaderType shaderType, GLuint& programID) {

	programID = glCreateShader(shaderType);
	glShaderSource(programID, 1, &shaderSource, NULL);
	glCompileShader(programID);
	glGetShaderiv(programID, GL_COMPILE_STATUS, &m_success);
	if (!m_success) {
		glGetShaderInfoLog(programID, 512, NULL, m_infoLog);
		std::cerr << "ERROR: vertex shader compilation failed:\n" << m_infoLog << std::endl;
		FILE_LOG(logERROR) << m_infoLog;
	}
}
