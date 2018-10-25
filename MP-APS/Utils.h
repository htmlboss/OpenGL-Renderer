#pragma once

// Makes a given class move-only by disabling
// copy ctor anc copy assignment
#define MAKE_MOVE_ONLY(classname) \
	classname(const classname&) = delete; \
	classname& operator=(const classname&) = delete; \
	classname(classname&&) = default; \
	classname& operator=(classname&&) = default;
