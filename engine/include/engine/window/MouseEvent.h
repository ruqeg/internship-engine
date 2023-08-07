#pragma once

#include <engine/types.h>

#include <DirectXMath.h>

namespace window
{

struct MousePoint
{
	int32 x;
	int32 y;
	inline operator DirectX::XMVECTOR() const noexcept { return { static_cast<float>(x), static_cast<float>(y) }; }
};

class MouseEvent
{
public:
	enum EventType
	{
		LPress,
		LRealese,
		RPress,
		RRealese,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RawMove,
		Invalid
	};

public:
	MouseEvent();
	MouseEvent(const EventType type, const int32 x, const int32 y);
	bool isVaild() const;
	EventType getType() const;
	MousePoint getPos() const;
	int32 getPosX() const;
	int32 getPosY() const;

private:
	EventType type;
	int32 x;
	int32 y;
};

}