#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "DX12Base.hpp"

namespace dx {
    class Renderer
    {
      public:
        Renderer()  = default;
        ~Renderer() = default;

        void Init(HWND window);
        void Update();
        void KeyDown(WPARAM wparam);
        void Resize(int width, int height);

      protected:
        DX12Base dx12Base;
    };
}  // namespace dx