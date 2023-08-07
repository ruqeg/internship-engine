#pragma once

#include <windows.h>
#include <windowsx.h>
#include <stdexcept>

#include <engine/types.h>
#include <engine/utils/StringConverter.h>
#include <engine/utils/ErrorLogger.h>

namespace window
{

class WindowContainer;

class RenderWindow
{
public:
	RenderWindow() = default;
	~RenderWindow();
	RenderWindow(RenderWindow&) = delete;
	RenderWindow(RenderWindow&&) = delete;
	RenderWindow& operator=(RenderWindow&) = delete;
	RenderWindow& operator=(RenderWindow&&) = delete;

	void initialize(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int32 width, int32 height);
	bool processMessages();
	void flush(const void** buffer, int32 bufferWidth, int32 bufferHeight);

	HWND GetHWND() const;

private:
	void registerWindowClass();

private:
	HWND handle = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle;
	std::wstring windowTitleWide;
	std::string windowClass;
	std::wstring windowClassWide;
};

}