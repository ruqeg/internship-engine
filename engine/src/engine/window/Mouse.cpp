#include <engine/window/Mouse.h>

namespace window
{

void Mouse::onLeftPressed(int32 x, int32 y)
{
	leftIsDown = true;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
}

void Mouse::onLeftReleased(int32 x, int32 y)
{
	leftIsDown = false;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::LRealese, x, y));
}

void Mouse::onRightPressed(int32 x, int32 y)
{
	rightIsDown = true;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void Mouse::onRightReleased(int32 x, int32 y)
{
	rightIsDown = false;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::RRealese, x, y));
}

void Mouse::onMiddlePressed(int32 x, int32 y)
{
	middleIsDown = true;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void Mouse::onMiddleReleased(int32 x, int32 y)
{
	middleIsDown = false;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void Mouse::onWheelUp(int32 x, int32 y)
{
	eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void Mouse::onWheelDown(int32 x, int32 y)
{
	eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void Mouse::onMouseMove(int32 x, int32 y)
{
	this->x = x;
	this->y = y;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void Mouse::onMouseMoveRaw(int32 x, int32 y)
{
	eventBuffer.push(MouseEvent(MouseEvent::EventType::RawMove, x, y));
}

bool Mouse::isLeftDown() const
{
	return leftIsDown;
}

bool Mouse::isMiddleDown() const
{
	return middleIsDown;
}

bool Mouse::isRightDown() const
{
	return rightIsDown;
}

int32 Mouse::getPosX() const
{
	return x;
}

int32 Mouse::getPosY() const
{
	return y;
}

MousePoint Mouse::getPos() const
{
	return { x, y };
}

bool Mouse::eventBufferIsEmpty() const
{
	return eventBuffer.empty();
}

MouseEvent Mouse::readEvent()
{
	if (eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = eventBuffer.front();
		eventBuffer.pop();
		return e;
	}
}

}