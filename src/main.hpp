#pragma once
#define WIN32_LEAN_AND_MEAN
#include "dx12base/renderer.hpp"
#include <Windows.h>
#include <memory>

std::unique_ptr<dx::Renderer> g_Renderer;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND             CreateWin32Window();
void             SetFullscreen(HWND window);