#pragma once
#include <string_view>

/***********************************************************************************/
// Forward Declarations
namespace pugi {
	class xml_node;
}

class Camera;

/***********************************************************************************/
class ISystem {

public:
	enum State : std::size_t {
		UNINITIALIZED,

		// Running systems
		RUNNING,
		PAUSED,

		// Dead systems
		SUCCEEDED,
		ABORTED
	};

	ISystem() noexcept : m_state{UNINITIALIZED} {}
	virtual ~ISystem() = default;

	auto GetState() const noexcept { return m_state; }
	
	auto IsRunning() const noexcept { return m_state == RUNNING; }
	auto IsPaused() const noexcept { return m_state == PAUSED; }
	auto IsDead() const noexcept { return m_state == SUCCEEDED || m_state == ABORTED; }

protected:
	virtual void OnInit(const pugi::xml_node& configNode) = 0;
	virtual void OnUpdate(const double delta) {}
	virtual void OnUpdate(const class Camera& camera, const double delta) {}
	virtual void OnSucceed() const = 0;
	virtual void OnAbort(const std::string_view error) = 0;

	void SetState(const State state) noexcept { m_state = state; }

private:
	State m_state;
};

