#include "ResourceLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <log.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

/***********************************************************************************/
std::string ResourceLoader::LoadTextFile(const std::string_view path) {
	std::string fileContents;
	std::ifstream file;
	std::stringstream fileSS;

	file.exceptions(std::ifstream::badbit);
	try {
		file.open(path.data());
		
		if (file) {
			// Read file's buffer contents into stream
			fileSS << file.rdbuf();
			// close file handlers
			file.close();
			// Convert stream into string
			fileContents = fileSS.str();

			return fileContents;
		}
		// Bad file
		throw std::runtime_error("Failed to open: " + std::string(path) + '\n');
	}
	catch (std::ifstream::failure& e) {
		std::cerr << "Shader error: " << e.what();
	}
	catch (std::runtime_error& e) {
		std::cerr << "Shader error: " << e.what();
	}
	catch (...) {
		std::cout << "Unhandled exception in Shader::readShader().\n";
	}
}

/***********************************************************************************/
unsigned char* ResourceLoader::LoadSTBImage(char const* filename, int* x, int* y, int* comp, const ColorMode mode) {

	auto data = stbi_load(filename, x, y, comp, mode);

	if (data == nullptr) {
		const auto error = "stb_image error (" + std::string(filename) + "): " + stbi_failure_reason();
		FILE_LOG(logERROR) << error;
		throw std::runtime_error(error);
	}

	return data;
}
