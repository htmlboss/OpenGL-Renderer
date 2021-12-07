#pragma once

#include "GLShaderProgram.h"
#include "ShaderStage.h"

#include <optional>
#include <vector>

namespace Graphics {

class GLShaderProgramFactory {
public:
    static std::optional<GLShaderProgram> createShaderProgram(
        const std::string& programName, 
        const std::vector<Graphics::ShaderStage>& stages
    );
};

}; // namespace Graphics
