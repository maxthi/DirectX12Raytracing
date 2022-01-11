#pragma once
#include "dx12.hpp"

namespace dx {

    class DX12Base
    {
      public:
        DX12Base()  = default;
        ~DX12Base() = default;

        void Init();

      protected:
        uint32_t                          mNumFrames = 2;
        ComPtr<ID3D12Device2>             mDevice;
        ComPtr<ID3D12CommandQueue>        mCommandQueue;
        ComPtr<IDXGISwapChain4>           mSwapChain;
        ComPtr<ID3D12Resource>            mBackBuffers[mNumFrames];
        ComPtr<ID3D12GraphicsCommandList> mCommandList;
        ComPtr<ID3D12CommandAllocator>    mCommandAllocator;
        ComPtr<ID3D12DescriptorHeap>      mRTVDescriptorHeap;
        uint32_t                          mRTVDescriptorSize{0};
        uint32_t                          mCurrentBackBufferIndex{0};
    };
}  // namespace dx