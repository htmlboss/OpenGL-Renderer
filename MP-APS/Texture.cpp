#include "Texture.h"

/***********************************************************************************/
Texture::Texture(unsigned char* data, const size_t width, const size_t height, const size_t components, const size_t ID) :
	m_texID(ID),
	m_width(width),
	m_height(height),
	m_components(components),
	m_data(data) { data = nullptr; }

/***********************************************************************************/
Texture::Texture(Texture& other) : m_texID(other.m_texID),
								   m_width(other.m_width),
								   m_height(other.m_height),
								   m_components(other.m_components),
								   m_data(other.m_data) { other.m_data = nullptr; }

/*
/***********************************************************************************
Texture& Texture::operator=(const Texture& rhs) {
	m_data.reset(new unsigned char(*rhs.m_data));
	return *this;
}
*/
/***********************************************************************************
Texture& Texture::operator=(Texture&& rhs) {
	m_data = std::move(rhs.m_data);
	return *this;
}
*/
