#include "GLShader.h"

#include "../ResourceManager.h"

#include <iostream>
#include <unordered_map>

/***********************************************************************************/
const std::unordered_map<std::string_view, int> ShaderTypes {
	{"vertex", GL_VERTEX_SHADER },
	{"pixel", GL_FRAGMENT_SHADER },
	{"fragment", GL_FRAGMENT_SHADER },
	{"geometry", GL_GEOMETRY_SHADER },
	{"compute", GL_COMPUTE_SHADER },
	{"tessctrl", GL_TESS_CONTROL_SHADER },
	{"tesseval", GL_TESS_EVALUATION_SHADER }
};

/***********************************************************************************/
GLShader::GLShader(const std::string_view path, const int type) {

	m_shaderID = glCreateShader(type);

	auto shaderCode = ResourceManager::GetInstance().LoadTextFile(path);

	scanForIncludes(shaderCode);
	replaceAll(shaderCode, "hash ", "#");

	compile(shaderCode.c_str());
}

/***********************************************************************************/
GLShader::GLShader(const std::string_view path, const std::string_view type) {

	m_shaderID = glCreateShader(ShaderTypes.at(type));

	auto shaderCode = ResourceManager::GetInstance().LoadTextFile(path);

	scanForIncludes(shaderCode);
	replaceAll(shaderCode, "hash ", "#");
	
	compile(shaderCode.c_str());
}

/***********************************************************************************/
void GLShader::AttachShader(const GLuint Program) const {
	glAttachShader(Program, m_shaderID);
}

/***********************************************************************************/
void GLShader::DetachShader(const GLuint Program) const {
	glDetachShader(Program, m_shaderID);
}

/***********************************************************************************/
void GLShader::DeleteShader() const {
	glDeleteShader(m_shaderID);
}

/***********************************************************************************/
void GLShader::scanForIncludes(std::string& shaderCode) {
	std::size_t startPos = 0;
	const static std::string include_dir = "#include ";

	// Scan string for all instances of include directive
	while ((startPos = shaderCode.find(include_dir, startPos)) != std::string::npos) {
		// Find position of include directive
		const auto pos = startPos + include_dir.length() + 1;
		const auto length = shaderCode.find('"', pos);
		const auto pathToIncludedFile = shaderCode.substr(pos, length - pos);

		// Load included file
		const auto includedFile = ResourceManager::GetInstance().LoadTextFile(pathToIncludedFile) + "\n";
		// Insert into shader code
		shaderCode.replace(startPos, (length + 1) - startPos, includedFile);
		
		// Increment start position and continue scanning
		startPos += includedFile.length();
	}
}

/***********************************************************************************/
void GLShader::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	std::size_t start_pos = 0;

	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

/***********************************************************************************/
void GLShader::compile(const GLchar* shaderCode) {
#ifdef _DEBUG
	std::cout << "Compiling shader file...\n";
#endif

	glShaderSource(m_shaderID, 1, &shaderCode, nullptr);
	glCompileShader(m_shaderID);

	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &m_success);
	if (!m_success) {
		glGetShaderInfoLog(m_shaderID, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "Shader Error: " << m_infoLog.data() << std::endl;
	}

#ifdef _DEBUG
	std::cout << "Compilation successful!\n";
#endif
}
