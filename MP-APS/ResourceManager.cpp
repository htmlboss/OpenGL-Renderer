#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <string_view>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

const static std::filesystem::path COMPRESSED_TEX_DIR{ std::filesystem::current_path() / "Data/cache/textures" };

struct CompressedImageDesc {
	GLint width{ -1 };
	GLint height{ -1 };
	GLint size{ -1 };
	GLint format{ -1 };
	unsigned char* data{ nullptr };
};

/***********************************************************************************/
void ResourceManager::ReleaseAllResources() {
	// Delete cached meshes
	for (auto& model : m_modelCache) {
		model.second->Delete();
	}

	// Deletes textures
	for (auto& tex : m_textureCache) {
		glDeleteTextures(1, &tex.second);
	}
}

/***********************************************************************************/
std::string ResourceManager::LoadTextFile(const std::filesystem::path& path) const {
	std::ifstream in(path, std::ios::in);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if (!in) {
		std::cerr << "Resource Manager: File loading error: " + path.string() << " " << errno << std::endl;
		std::abort();
	}

	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

/***********************************************************************************/
unsigned int ResourceManager::LoadHDRI(const std::string_view path) const {
	//stbi_set_flip_vertically_on_load(true);
	
	// Dont flip HDR otherwise the probe will be upside down. We flip the y-coord in the
	// shader to correctly render the texture.
	int width, height, nrComp;
	auto* data{ stbi_loadf(path.data(), &width, &height, &nrComp, 0) };
	
	//stbi_set_flip_vertically_on_load(false);

	if (!data) {
		std::cerr << "Resource Manager: Failed to load HDRI." << std::endl;
		std::abort();
	}

	unsigned int hdrTexture{ 0 };
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

#ifdef _DEBUG
	std::cout << "Resource Manager: Loaded HDR: " << path << std::endl;
#endif

	return hdrTexture;
}

/***********************************************************************************/
auto buildTextureCachePath(const std::filesystem::path& filenameNoExt) {
	const std::filesystem::path filename{ filenameNoExt.string() + ".bin" };
	return std::filesystem::path( COMPRESSED_TEX_DIR / filename);
}

/***********************************************************************************/
void saveCompressedImageToDisk(const std::filesystem::path& target, const CompressedImageDesc& desc) {
	if (!std::filesystem::exists(COMPRESSED_TEX_DIR)) {
		if (!std::filesystem::create_directories(COMPRESSED_TEX_DIR)) {
			std::cerr << "Failed to create texture cache directory: " << COMPRESSED_TEX_DIR << '\n';
			return;
		}
	}

	std::ofstream out(target, std::ios::binary);
	if (out) {
		out.write((char*)(&desc.size), sizeof(GLint));
		out.write((char*)(&desc.width), sizeof(GLint));
		out.write((char*)(&desc.height), sizeof(GLint));
		out.write((char*)(&desc.format), sizeof(GLint));
		out.write((char*)(desc.data), desc.size);
	}
}

/***********************************************************************************/
unsigned int ResourceManager::LoadTexture(const std::filesystem::path& path, const bool useMipMaps, const bool useUnalignedUnpack) {

	if (path.filename().empty()) {
		return 0;
	}
	
	const auto compressedFilePath{ buildTextureCachePath(path.stem()) };
	const auto compressedImageExists{ std::filesystem::exists(compressedFilePath) };

	const std::filesystem::path pathToLoad = compressedImageExists ? compressedFilePath : path;

	// Check if texture is already loaded in memory
	if (const auto val = m_textureCache.find(pathToLoad); val != m_textureCache.end()) {
		// Found it
		return val->second;
	}
	
	if (useUnalignedUnpack) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (compressedImageExists) {
		std::ifstream in(compressedFilePath, std::ios::binary);
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		if (!in) {
			return 0;
		}

		GLint size{ -1 };
		GLint width{ -1 };
		GLint height{ -1 };
		GLint format{ -1 };

		in.read((char*)&size, sizeof(GLint));
		if (in.fail() || size == -1) {
			return 0;
		}

		in.read((char*)&width, sizeof(GLint));
		in.read((char*)&height, sizeof(GLint));
		in.read((char*)&format, sizeof(GLint));

		auto compressedData = std::make_unique<unsigned char[]>(size);
		in.read((char*)compressedData.get(), size);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, size, compressedData.get());
		
	} else {
		int width = 0, height = 0, nrComponents = 0;
		unsigned char* data = stbi_load(pathToLoad.c_str(), &width, &height, &nrComponents, 0);
		if (!data) {
			std::cerr << "Failed to load texture: " << path << std::endl;
			glDeleteTextures(1, &textureID);
			stbi_image_free(data);
			return 0;
		}

		GLenum format = 0;
		GLenum internalFormat = 0;
		switch (nrComponents) {
		case 1:
			format = GL_RED;
			internalFormat = GL_COMPRESSED_RED;
			break;
		case 3:
			format = GL_RGB;
			internalFormat = GL_COMPRESSED_RGB;
			break;
		case 4:
			format = GL_RGBA;
			internalFormat = GL_COMPRESSED_RGBA;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		
		glHint(GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		GLint compressed = GL_FALSE;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
		if (compressed == GL_TRUE) {
			GLint compressedSize = -1;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);

			GLint internalFormat = -1;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

			auto compressedData = std::make_unique<unsigned char[]>(compressedSize);
			glGetCompressedTexImage(GL_TEXTURE_2D, 0, (GLvoid*)compressedData.get());

			const CompressedImageDesc desc {
				.width = width,
				.height = height,
				.size = compressedSize,
				.format = internalFormat,
				.data = compressedData.get()
			};

			saveCompressedImageToDisk(compressedFilePath, desc);
		}
	}
	
	if (useMipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	if (useUnalignedUnpack) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
	}

	return m_textureCache.try_emplace(path, textureID).first->second;
}

/***********************************************************************************/
std::vector<char> ResourceManager::LoadBinaryFile(const std::string_view path) const {
	std::ifstream in(path.data(), std::ios::binary);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if (!in) {
		std::cerr << "Resource Manager: Failed to load binary file: " << path << std::endl;
		std::abort();
	}

#ifdef _DEBUG
	std::cout << "Resource Manager: loaded binary file: " << path << std::endl;
#endif

	// Copy and return binary contents into vector
	return std::vector<char>(std::istreambuf_iterator<char>(in), (std::istreambuf_iterator<char>()));
}

/***********************************************************************************/
ModelPtr ResourceManager::GetModel(const std::string_view name, const std::string_view path) {

	// Check if model is already loaded.
	const auto val = m_modelCache.find(path.data());

	if (val == m_modelCache.end()) {
		// Load model, cache it, and return a shared_ptr to it.
		return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(path, name)).first->second;
	}

	return val->second;
}

/***********************************************************************************/
ModelPtr ResourceManager::CacheModel(const std::string_view name, const Model model, const bool overwriteIfExists) {
	if (overwriteIfExists) {
		return m_modelCache.insert_or_assign(name.data(), std::make_shared<Model>(model)).first->second;
	}
	return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(model)).first->second;
}

/***********************************************************************************/
std::optional<PBRMaterialPtr> ResourceManager::GetMaterial(const std::string_view name) const {
	
	// Check if material exists
	const auto val = m_materialCache.find(name.data());

	if (val == m_materialCache.end()) {
		return std::optional<PBRMaterialPtr>();
	}

	return std::make_optional<PBRMaterialPtr>(val->second);
}

/***********************************************************************************/
PBRMaterialPtr ResourceManager::CacheMaterial(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath, const std::string_view metallicPath, const std::string_view normalPath, const std::string_view roughnessPath, const std::string_view alphaMaskPath) {
	auto mat = PBRMaterial();
	mat.Init(name, albedoPath, aoPath, metallicPath, normalPath, roughnessPath, alphaMaskPath);

	return m_materialCache.try_emplace(name.data(), std::make_shared<PBRMaterial>(mat)).first->second;
}

/***********************************************************************************/
void ResourceManager::UnloadModel(const std::string_view modelName) {
	const auto model = m_modelCache.find(modelName.data());

	if (model != m_modelCache.end()) {
		model->second->Delete();

		m_modelCache.erase(modelName.data());
	}
}
