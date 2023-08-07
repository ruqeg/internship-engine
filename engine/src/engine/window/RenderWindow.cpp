#include <engine/window/WindowContainer.h>

namespace window
{

RenderWindow::~RenderWindow()
{
	if (handle != NULL)
	{
		UnregisterClass(windowClassWide.c_str(), hInstance);
		DestroyWindow(handle);
	}
}

void RenderWindow::initialize(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int32 width, int32 height)
{
	this->hInstance = hInstance;
	this->windowTitle = windowTitle;
	windowTitleWide = utils::StringConverter::stringToWide(this->windowTitle);
	this->windowClass = windowClass;
	windowClassWide = utils::StringConverter::stringToWide(this->windowClass);

	registerWindowClass();

	handle = CreateWindowEx(NULL,
		windowClassWide.c_str(),
		windowTitleWide.c_str(),
		DFC_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		(LPVOID)pWindowContainer);

	if (!handle)
	{
		utils::ErrorLogger::log("CreateWindowEX failed for window." + this->windowTitle);
		exit(-1);
	}

	ShowWindow(handle, SW_SHOW);
	SetForegroundWindow(handle);
	SetFocus(handle);
}

LRESULT CALLBACK handleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		return 0;
	}
	default:
	{
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->windowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK handleMsgSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			utils::ErrorLogger::log("Critical Error: Pointer to window container is null during WN_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(handleMsgRedirect));
		return pWindow->windowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RenderWindow::registerWindowClass()
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = handleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClassWide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	if (!RegisterClassEx(&wc))
	{
		utils::ErrorLogger::log(GetLastError(), "Failed to register window class.");
		exit(-1);
	}
}

bool RenderWindow::processMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(handle))
		{
			handle = NULL;
			UnregisterClass(windowClassWide.c_str(), hInstance);
			return false;
		}
	}

	return true;
}

void RenderWindow::flush(const void** buffer, int32 bufferWidth, int32 bufferHeight)
{
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bufferWidth;
	bmi.bmiHeader.biHeight = -bufferHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	RECT rect;
	int32 width = 0;
	int32 height = 0;
	if (GetWindowRect(handle, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	StretchDIBits(
		GetDC(handle),
		0, 0, width, height,
		0, 0, bufferWidth, bufferHeight,
		buffer,
		&bmi,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

HWND RenderWindow::GetHWND() const
{
	return handle;
}

}