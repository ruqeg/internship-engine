#pragma once

#include <engine/types.h>

namespace window
{

class WindowEvent
{
public:
	enum EventType
	{
		Resized,
		Invalid
	};
public:
	WindowEvent();
	WindowEvent(const EventType type);
	bool isResized() const;
private:
	EventType type;
};

}