#pragma once

#include <engine/window/KeyboardEvent.h>

#include <array>
#include <queue>

namespace window
{

class Keyboard
{
public:
	Keyboard();
	bool keyIsPressed(const uint8 keycode) const;
	bool keyBufferIsEmpty() const;
	bool charBufferIsEmpty() const;
	KeyboardEvent readKey();
	uint8 readChar();
	void enableAutoRepeatKeys();
	void disableAutoRepeatKeys();
	void enbaleAutoRepeatChars();
	void desableAutoRepeatChars();
	bool isKeyAutoRepeat() const;
	bool isCharsAutoRepeat() const;

private:
	void onKeyPressed(const uint8 key);
	void onKeyReleased(const uint8 key);
	void onChar(const uint8 key);

private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	std::array<bool, 256> keyState;
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<uint8> charBuffer;

	friend class WindowContainer;
};

}