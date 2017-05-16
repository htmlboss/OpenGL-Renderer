#pragma once
#include <string>

class IRenderComponent {
public:

	constexpr IRenderComponent(const std::string_view name, const std::size_t width, const std::size_t height) noexcept :	m_name(name),
																															m_width(width),
																															m_height(height) {}

	virtual ~IRenderComponent() = default;

	virtual void Resize(const std::size_t width, const std::size_t height) = 0;

protected:
	const std::string m_name;
	std::size_t m_width, m_height;
};

