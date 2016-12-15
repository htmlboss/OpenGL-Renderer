#pragma once
#include <string>

class ResourceLoader {
public:
	ResourceLoader();
	~ResourceLoader();

	static std::string LoadTextFile(const std::string& path);
};

