#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace dx {
	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

		void Init(HWND window);
		void Update();
		void KeyDown(WPARAM wparam);
		void Resize(int width, int height);
	};
}