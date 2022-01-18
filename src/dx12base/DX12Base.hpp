#pragma once
#include "dx12.hpp"
#include <chrono>
namespace dx {

    class DX12Base
    {
      public:
        DX12Base()  = default;
        ~DX12Base() = default;

        void Init();

        void                              EnableDebugLayer();
        ComPtr<IDXGIAdapter4>             QueryDx12Adapters(bool useWarp);
        ComPtr<ID3D12Device2>             CreateDevice(ComPtr<IDXGIAdapter4> adapter);
        ComPtr<ID3D12CommandQueue>        CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        bool                              CheckTearingSupport();
        ComPtr<IDXGISwapChain4>           CreateSwapChain(HWND hWNd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
        ComPtr<ID3D12DescriptorHeap>      CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
        void                              UpdateRenderTargetViews(ComPtr<ID3D12Device2> device, ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap);
        ComPtr<ID3D12CommandAllocator>    CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);
        ComPtr<ID3D12Fence>               CreateFence(ComPtr<ID3D12Device2> device);
        HANDLE                            CreateEventHandle();
        uint64_t                          Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue);

        void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
        void Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent);

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