#include <engine/window/Keyboard.h>

namespace window
{

Keyboard::Keyboard()
{
	keyState.fill(false);
}

bool Keyboard::keyIsPressed(const uint8 keycode) const
{
	return keyState[keycode];
}

bool Keyboard::keyBufferIsEmpty() const
{
	return keyBuffer.empty();
}

bool Keyboard::charBufferIsEmpty() const
{
	return charBuffer.empty();
}

KeyboardEvent Keyboard::readKey()
{
	if (keyBuffer.empty())
	{
		return KeyboardEvent();
	}
	else
	{
		KeyboardEvent event = keyBuffer.front();
		keyBuffer.pop();
		return event;
	}
}

uint8 Keyboard::readChar()
{
	if (charBuffer.empty())
	{
		return 0u;
	}
	else
	{
		uint8 symbol = charBuffer.front();
		charBuffer.pop();
		return symbol;
	}
}

void Keyboard::onKeyPressed(const uint8 key)
{
	keyState[key] = true;
	keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void Keyboard::onKeyReleased(const uint8 key)
{
	keyState[key] = false;
	keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void Keyboard::onChar(const uint8 key)
{
	charBuffer.push(key);
}

void Keyboard::enableAutoRepeatKeys()
{
	autoRepeatKeys = true;
}

void Keyboard::disableAutoRepeatKeys()
{
	autoRepeatKeys = false;
}

void Keyboard::enbaleAutoRepeatChars()
{
	autoRepeatChars = true;
}

void Keyboard::desableAutoRepeatChars()
{
	autoRepeatChars = false;
}

bool Keyboard::isKeyAutoRepeat() const
{
	return autoRepeatKeys;
}

bool Keyboard::isCharsAutoRepeat() const
{
	return autoRepeatChars;
}

}