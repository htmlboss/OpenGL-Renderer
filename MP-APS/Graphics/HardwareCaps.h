#pragma once

#include <ostream>

namespace Graphics {

struct HardwareCaps {
    float MaxAnisotropy;
	int MaxArrayTextureLayers;
	int MaxTextureSamples;
	int MaxTextureSamplers;
	int MaxVertexUniformBlocks;
	int MaxGeometryUniformBlocks;
	int MaxFragmentUniformBlocks;
	int MaxComputeWorkGroupSize;
	int MaxComputeWorkGroupCount;
	int TotalVideoMemoryKB;
};

}

inline std::ostream& operator<<(std::ostream& out, const Graphics::HardwareCaps& c) {
    return out << c.TotalVideoMemoryKB;
}
