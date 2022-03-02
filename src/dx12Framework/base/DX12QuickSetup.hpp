#pragma once

#include "DX12Base.hpp"

namespace dx {
    class DX12QuickSetup
    {
      public:
        DX12QuickSetup() = default;
        ~DX12QuickSetup() = default;

        void Init(HWND window, uint32_t width, uint32_t height);

        DX12Base mDX12Base;

        static const uint32_t             sNumFrames = 2;
        Microsoft::WRL::ComPtr<IDXGIAdapter4>             mAdapter;
        Microsoft::WRL::ComPtr<ID3D12Device2>             mDevice;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;
        Microsoft::WRL::ComPtr<IDXGISwapChain4>           mSwapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource>            mBackBuffers[sNumFrames];
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    mCommandAllocators[sNumFrames];
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>      mRTVDescriptorHeap;
        uint32_t                          mRTVDescriptorSize{0};
        uint32_t                          mCurrentBackBufferIndex{0};

        // synchronisation (should this be handled by the application?)
        Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
        uint64_t            mFenceValue                    = 0;
        uint64_t            mFrameFenceValues[sNumFrames] = {};
        HANDLE              mFenceEvent;
    };
}  // namespace dx