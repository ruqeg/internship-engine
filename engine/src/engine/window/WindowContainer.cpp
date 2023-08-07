#include <engine/window/WindowContainer.h>

namespace window
{

WindowContainer::WindowContainer()
{
	static bool rawInputInitialized = false;
	if (rawInputInitialized == false)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			utils::ErrorLogger::log(GetLastError(), "Failed to register raw input devices.");
			exit(-1);
		}

		rawInputInitialized = true;
	}
}

LRESULT WindowContainer::windowProcEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		onWindowResized();
		return 0;
		//Keyboard
	case WM_KEYDOWN:
	{
		const uint8 keycode = static_cast<uint8>(wParam);
		if (keyboard.isKeyAutoRepeat())
		{
			keyboard.onKeyPressed(keycode);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard.onKeyPressed(keycode);
			}
		}
		return 0;
	}
	case WM_KEYUP:
	{
		const uint8 keycode = static_cast<uint8>(wParam);
		keyboard.onKeyReleased(keycode);
		return 0;
	}
	case WM_CHAR:
	{
		const uint8 ch = static_cast<uint8>(wParam);
		if (keyboard.isCharsAutoRepeat())
		{
			keyboard.onChar(ch);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard.onChar(ch);
			}
		}
		return 0;
	}
	//Mouse
	case WM_MOUSEMOVE:
	{
		int32 x = LOWORD(lParam);
		int32 y = HIWORD(lParam);
		mouse.onMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int32 x = LOWORD(lParam);
		int32 y = HIWORD(lParam);
		mouse.onLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int32 x = LOWORD(lParam);
		int32 y = HIWORD(lParam);
		mouse.onRightPressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int32 x = LOWORD(lParam);
		int32 y = HIWORD(lParam);
		mouse.onLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int32 x = LOWORD(lParam);
		int32 y = HIWORD(lParam);
		mouse.onRightReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int32 x = LOWORD(lParam);
		int32 y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.onWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.onWheelDown(x, y);
		}
		return 0;
	}
	case WM_INPUT:
	{
		// TODO why don't work???
		UINT dataSize;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					mouse.onMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void WindowContainer::onWindowResized()
{
	eventsBuffer.push(WindowEvent(WindowEvent::EventType::Resized));
}

bool WindowContainer::windowEventsBufferIsEmpty() const
{
	return eventsBuffer.empty();
}

WindowEvent WindowContainer::readWindowEvent()
{
	if (eventsBuffer.empty())
	{
		return WindowEvent();
	}
	else
	{
		WindowEvent event = eventsBuffer.front();
		eventsBuffer.pop();
		return event;
	}
}

}