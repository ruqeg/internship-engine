#pragma once

#include <engine/window/RenderWindow.h>
#include <engine/window/Keyboard.h>
#include <engine/window/Mouse.h>
#include <engine/window/WindowEvent.h>

#include <memory>

namespace window
{

class WindowContainer
{
public:
	WindowContainer();
	WindowContainer(const WindowContainer&) = delete;
	WindowContainer(const WindowContainer&&) = delete;
	WindowContainer& operator=(const WindowContainer&) = delete;
	WindowContainer&& operator=(const WindowContainer&&) = delete;

	virtual LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	LRESULT windowProcEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	bool windowEventsBufferIsEmpty() const;
	WindowEvent readWindowEvent();

private:
	void onWindowResized();

protected:
	window::RenderWindow renderWindow;
	window::Keyboard keyboard;
	window::Mouse mouse;
private:
	std::queue<WindowEvent> eventsBuffer;
};

}