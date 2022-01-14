#pragma once
#include "dx12.hpp"

namespace dx {

    class DX12Base
    {
      public:
        DX12Base()  = default;
        ~DX12Base() = default;

        void Init();

        void EnableDebugLayer();
        ComPtr<IDXGIAdapter4> QueryDx12Adapters(bool useWarp);
        ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter);
        ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);

      protected:
        static const uint32_t             sNumFrames = 2;
        ComPtr<ID3D12Device2>             mDevice;
        ComPtr<ID3D12CommandQueue>        mCommandQueue;
        ComPtr<IDXGISwapChain4>           mSwapChain;
        ComPtr<ID3D12Resource>            mBackBuffers[sNumFrames];
        ComPtr<ID3D12GraphicsCommandList> mCommandList;
        ComPtr<ID3D12CommandAllocator>    mCommandAllocator;
        ComPtr<ID3D12DescriptorHeap>      mRTVDescriptorHeap;
        uint32_t                          mRTVDescriptorSize{0};
        uint32_t                          mCurrentBackBufferIndex{0};
    };
}  // namespace dx