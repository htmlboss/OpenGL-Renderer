#pragma once

// https://medium.com/@mscansian/a-with-navigation-meshes-246fd9e72424
// https://stackoverflow.com/questions/2439283/how-can-i-create-min-stl-priority-queue

class AISystem {
public:

	AISystem() = default;

	AISystem(AISystem&&) = delete;
	AISystem(const AISystem&) = delete;
	AISystem& operator=(AISystem&&) = delete;
	AISystem& operator=(const AISystem&) = delete;

	~AISystem() = default;

private:

};
