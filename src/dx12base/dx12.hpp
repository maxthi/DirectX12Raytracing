#pragma once
#include <wrl.h>

using namespace Microsoft::WRL;

// dx12 header contains all direct3d 12 objects (device, command queue, etc.)
#include <d3d12.h>

// Microsoft DX Graphics Infrastructure DXGI
// manage low level tasks, such as enumerating GPU adapters
// present rendered image to screen
// handle full screen transitions
// detect HDR displays
#include <dxgi1_6.h>

// compile HLSL shaders at runtime
#include <d3dcompiler.h>

// SIMD friendly C++ types and functions for commonly used
// graphics related programming
#include <DirectXMath.h>

// d3d12 extension library
// additional functionality
#include <directx/d3dx12.h>


inline void ThrowIfFailed(HRESULT hr)
{
    if(FAILED(hr))
    {
        throw std::exception();
    }
}