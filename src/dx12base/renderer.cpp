#include "renderer.hpp"

namespace dx {
    void Renderer::Init(HWND window) { dx12Base.Init(); }
    void Renderer::Update() {}
    void Renderer::KeyDown(WPARAM wparam) {}
    void Renderer::Resize(int width, int height) {}
}  // namespace dx