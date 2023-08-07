#pragma once

#include <engine/window/Keyboard.h>
#include <engine/window/Mouse.h>
#include <engine/window/WindowContainer.h>
#include <DirectXMath.h>

namespace engine
{

class Engine : public window::WindowContainer
{
public:
	Engine() = default;
	Engine(const Engine&) = delete;
	Engine(const Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine&& operator=(const Engine&&) = delete;
};

}