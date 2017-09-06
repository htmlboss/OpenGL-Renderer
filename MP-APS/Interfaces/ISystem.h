#pragma once

/***********************************************************************************/
// Forward Declarations
namespace pugi {
	class xml_node;
}

class Camera;;

/***********************************************************************************/
class ISystem {
public:
	ISystem() = default;
	virtual ~ISystem() = default;

	virtual void Init(const pugi::xml_node& configNode) = 0;
	virtual void Update(const double delta) {}
	virtual void Update(const class Camera& camera, const double delta) {}
};

