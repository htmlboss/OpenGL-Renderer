#include "ResourceLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

/***********************************************************************************/
ResourceLoader::ResourceLoader() {
}

/***********************************************************************************/
ResourceLoader::~ResourceLoader() {
}

/***********************************************************************************/
std::string ResourceLoader::LoadTextFile(const std::string & path) {
	std::string fileContents;
	std::ifstream file;
	std::stringstream fileSS;

	file.exceptions(std::ifstream::badbit);
	try {
		file.open(path);
		
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
		throw std::runtime_error("Failed to open: " + path + '\n');
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
