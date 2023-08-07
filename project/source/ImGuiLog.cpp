#include "ImGuiLog.h"

ImGuiLog& ImGuiLog::getInstance()
{
    static ImGuiLog instance;
    return instance;
}

void ImGuiLog::clear()
{
    buffer.clear();
}

void ImGuiLog::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void ImGuiLog::addLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    buffer.appendfv(fmt, args);
    va_end(args);

    scrollToBottom = true;
}

void ImGuiLog::draw(const char* title, bool* p_opened)
{
    //ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin(title, p_opened);
    ImGui::TextUnformatted(buffer.begin());

    if (scrollToBottom)
    {
        ImGui::SetScrollHereY(1.0f);
    }
    scrollToBottom = false;
    ImGui::End();
}