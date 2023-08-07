#pragma once

#include <engine/window/MouseEvent.h>

#include <queue>

namespace window
{

class Mouse
{
public:
	bool isLeftDown() const;
	bool isMiddleDown() const;
	bool isRightDown() const;

	int32 getPosX() const;
	int32 getPosY() const;
	MousePoint getPos() const;

	bool eventBufferIsEmpty() const;
	MouseEvent readEvent();

private:
	void onLeftPressed(int32 x, int32 y);
	void onLeftReleased(int32 x, int32 y);
	void onRightPressed(int32 x, int32 y);
	void onRightReleased(int32 x, int32 y);
	void onMiddlePressed(int32 x, int32 y);
	void onMiddleReleased(int32 x, int32 y);
	void onWheelUp(int32 x, int32 y);
	void onWheelDown(int32 x, int32 y);
	void onMouseMove(int32 x, int32 y);
	void onMouseMoveRaw(int32 x, int32 y);

private:
	std::queue<MouseEvent> eventBuffer;
	bool leftIsDown = false;
	bool rightIsDown = false;
	bool middleIsDown = false;
	int32 x = 0;
	int32 y = 0;

	friend class WindowContainer;
};

}