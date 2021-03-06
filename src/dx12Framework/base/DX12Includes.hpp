#pragma once
#include <wrl.h>

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

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
//#include <directx/d3dx12.h>
#include "../../../external/directx/d3dx12.h" // TODO: FIX ME

// windows functionality
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// include std::exception
#include <exception>


inline void ThrowIfFailed(HRESULT hr)
{
    if(FAILED(hr))
    {
        throw std::exception();
    }
}