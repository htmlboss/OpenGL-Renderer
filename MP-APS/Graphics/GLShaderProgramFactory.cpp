#include "GLShaderProgramFactory.h"

#include "../ResourceManager.h"

#include <glad/glad.h>

#include <fmt/core.h>

#include <iostream>
#include <unordered_map>

namespace Graphics {

/***********************************************************************************/
// Move this to ShaderStage or something
const std::unordered_map<std::string, int> Type2GL_ENUM {
    { "vertex", GL_VERTEX_SHADER },
    { "fragment", GL_FRAGMENT_SHADER },
    { "geometry", GL_GEOMETRY_SHADER },
	{ "compute", GL_COMPUTE_SHADER }
};

/***********************************************************************************/
void compile(const GLuint id, const GLchar* shaderCode) {
	glShaderSource(id, 1, &shaderCode, nullptr);
	glCompileShader(id);
}

/***********************************************************************************/
bool compileStage(const GLuint id, const std::string& shaderCode) {
    GLint success{ GL_FALSE };
	GLint logLength{ -1 };
	
	compile(id, shaderCode.c_str());
    
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	
    std::vector<GLchar> infoLog(0, logLength);
	glGetShaderInfoLog(id, logLength, nullptr, infoLog.data());
	std::cout << fmt::format("{}", std::string(infoLog.cbegin(), infoLog.cend())) << std::endl;

    return success == GL_TRUE;
}

/***********************************************************************************/
bool linkProgram(const GLuint id) {
    GLint success{ GL_FALSE };
	GLint logLength{ -1 };
    
    glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

    std::vector<GLchar> infoLog(0, logLength);
	glGetProgramInfoLog(id, logLength, nullptr, infoLog.data());
	std::cout << fmt::format("{}", std::string(infoLog.cbegin(), infoLog.cend())) << std::endl;

    return success == GL_TRUE;
}

/***********************************************************************************/
bool validateProgram(const GLuint id) {
    GLint success{ GL_FALSE };
	GLint logLength{ -1 };

    glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &success);
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

    std::vector<GLchar> infoLog(0, logLength);
    glGetProgramInfoLog(id, logLength, nullptr, infoLog.data());
	std::cout << fmt::format("{}", std::string(infoLog.cbegin(), infoLog.cend())) << std::endl;

    return success == GL_TRUE;
}

/***********************************************************************************/
void scanForIncludes(std::string& shaderCode) {
    std::size_t startPos = 0;
	const static std::string includeDirective{ "#include " };

    // Scan string for all instances of include directive
	while ((startPos = shaderCode.find(includeDirective, startPos)) != std::string::npos) {
		// Find position of include directive
		const auto pos = startPos + includeDirective.length() + 1;
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
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	std::size_t start_pos = 0;

	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

/***********************************************************************************/
std::optional<GLShaderProgram> GLShaderProgramFactory::createShaderProgram(
    const std::string& programName, 
    const std::vector<Graphics::ShaderStage>& stages
) {

	std::cout << "Building shader program " << programName << std::endl;

    std::vector<GLuint> shaderIds;

    bool success{ true };
	for (auto i = 0; i < stages.size(); ++i) {
		auto id{ glCreateShader(Type2GL_ENUM.at(stages[i].type)) };
		shaderIds.push_back(id);

        auto shaderCode{ ResourceManager::GetInstance().LoadTextFile(stages[i].filePath) };
        scanForIncludes(shaderCode);
		replaceAll(shaderCode, "hash ", "#");
		if (!compileStage(id, shaderCode)) {
            success = false;
            break;
		}
	}
    if (!success) {
		for (const auto id : shaderIds) {
			glDeleteShader(id);
		}
        return std::nullopt;
    }

    GLuint programID{ glCreateProgram() };
    
    for (const auto id : shaderIds) {
        glAttachShader(programID, id);
    }
	// link and validate
    if (!linkProgram(programID) || !validateProgram(programID)) {
		for (const auto id : shaderIds) {
			glDetachShader(programID, id);
			glDeleteShader(id);
		}
		glDeleteProgram(programID);
        return std::nullopt;
    }

	for (const auto id : shaderIds) {
		glDetachShader(programID, id);
		glDeleteShader(id);
	}

	// leave uniform introspection to GLShaderProgram

	return std::make_optional<GLShaderProgram>({ programName, programID });
}

}; // namespace Graphics
