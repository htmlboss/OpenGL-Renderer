#pragma once
#include <string>

class IRenderComponent {
public:

	constexpr IRenderComponent(const std::string_view name, const size_t width, const size_t height) noexcept : m_name(name),
	                                                                                                            m_width(width),
	                                                                                                            m_height(height) {
	}

	virtual ~IRenderComponent() = default;

	virtual void Resize(const size_t width, const size_t height) = 0;

	auto GetName() const noexcept { return m_name; }

protected:
	const std::string m_name;
	size_t m_width, m_height;
};
