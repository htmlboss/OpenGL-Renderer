#pragma once

#include <memory>

class Texture {
public:
	Texture(unsigned char* data, const std::size_t width, const std::size_t height, const std::size_t components, const std::size_t ID);
	virtual ~Texture() { delete[] m_data; }

	Texture(Texture& other);
	Texture& operator=(const Texture& rhs);
	Texture& operator=(Texture&& rhs);

	bool operator==(const Texture& rhs) const { return this->m_texID == rhs.m_texID; }

	auto GetWidth() const { return m_width; }
	auto GetHeight() const { return m_height; }

	auto GetData() const { return m_data; }

private:
	const std::size_t m_texID;
	const std::size_t m_width, m_height, m_components;
	unsigned char* m_data;
};

