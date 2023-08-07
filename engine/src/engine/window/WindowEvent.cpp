#include <engine/window/WindowEvent.h>

namespace window
{

WindowEvent::WindowEvent()
	:
	type(WindowEvent::EventType::Invalid)
{}

WindowEvent::WindowEvent(const EventType type)
	:
	type(type)
{}

bool WindowEvent::isResized() const
{
	return type == WindowEvent::EventType::Resized;
}

}