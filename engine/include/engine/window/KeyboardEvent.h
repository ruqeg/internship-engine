#pragma once

#include <engine/types.h>

namespace window
{

class KeyboardEvent
{
public:
	enum EventType
	{
		Press,
		Release,
		Invalid
	};

public:
	KeyboardEvent();
	KeyboardEvent(const EventType type, const uint8 key);
	bool isPress() const;
	bool isRelease() const;
	bool isValid() const;
	uint8 getKeyCode() const;

private:
	EventType type;
	uint8 key;
};

}