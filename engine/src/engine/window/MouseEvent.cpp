#include <engine/window/MouseEvent.h>

namespace window
{

MouseEvent::MouseEvent()
	:
	type(EventType::Invalid),
	x(0),
	y(0)
{}

MouseEvent::MouseEvent(const EventType type, const int32 x, const int32 y)
	:
	type(type),
	x(x),
	y(y)
{}

bool MouseEvent::isVaild() const
{
	return type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::getType() const
{
	return type;
}

MousePoint MouseEvent::getPos() const
{
	return { x, y };
}

int32 MouseEvent::getPosX() const
{
	return x;
}

int32 MouseEvent::getPosY() const
{
	return y;
}

}