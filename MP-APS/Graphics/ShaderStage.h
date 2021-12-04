#pragma once

#include <string>

namespace Graphics {

struct ShaderStage {
    ShaderStage() noexcept = default;
    ShaderStage(const std::string& path, const std::string& type) :
        filePath{path}, type{type} {}

    std::string filePath;
    std::string type;
};

}; // namespace Graphics
