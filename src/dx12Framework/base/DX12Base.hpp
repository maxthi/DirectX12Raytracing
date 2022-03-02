#pragma once
#include "DX12Includes.hpp"
#include <chrono>

namespace dx {

    class DX12Base
    {
      public:
        DX12Base()  = default;
        ~DX12Base() = default;

        void                              EnableDebugLayer();
        ComPtr<IDXGIAdapter4>             QueryDx12Adapters(bool useWarp);
        ComPtr<ID3D12Device2>             CreateDevice(ComPtr<IDXGIAdapter4> adapter);
        ComPtr<ID3D12CommandQueue>        CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        bool                              CheckTearingSupport();
        ComPtr<IDXGISwapChain4>           CreateSwapChain(HWND hWNd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
        ComPtr<ID3D12DescriptorHeap>      CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
        void                              UpdateRenderTargetViews(ComPtr<ID3D12Device2>        device,
                                                                  ComPtr<IDXGISwapChain4>      swapChain,
                                                                  ComPtr<ID3D12DescriptorHeap> descriptorHeap,
                                                                  ComPtr<ID3D12Resource>*      backBuffers,
                                                                  uint32_t                     bufferCount);
        ComPtr<ID3D12CommandAllocator>    CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);
        ComPtr<ID3D12Fence>               CreateFence(ComPtr<ID3D12Device2> device);
        HANDLE                            CreateEventHandle();
        uint64_t                          Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue);

        void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
        void Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent);
    };
}  // namespace dx