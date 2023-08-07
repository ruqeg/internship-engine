#include <engine/window/KeyboardEvent.h>

namespace window
{

KeyboardEvent::KeyboardEvent()
	:
	type(KeyboardEvent::Invalid),
	key(0)
{}

KeyboardEvent::KeyboardEvent(const EventType type, const uint8 key)
	:
	type(type),
	key(key)
{}

bool KeyboardEvent::isPress() const
{
	return type == KeyboardEvent::Press;
}

bool KeyboardEvent::isRelease() const
{
	return type == KeyboardEvent::Release;
}

bool KeyboardEvent::isValid() const
{
	return type != KeyboardEvent::Invalid;
}

uint8 KeyboardEvent::getKeyCode() const
{
	return key;
}

}