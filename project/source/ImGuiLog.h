#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

class ImGuiLog
{
public:
	static ImGuiLog& getInstance();
public:
    void clear();
    void addLog(const char* fmt, ...);
    void setSize(int width, int height);
    void draw(const char* title, bool* p_opened = NULL);
private:
    ImGuiTextBuffer buffer;
    int width;
    int height;
    bool scrollToBottom;
private:
	ImGuiLog() = default;
};