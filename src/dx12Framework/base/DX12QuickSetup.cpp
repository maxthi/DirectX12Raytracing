#include "DX12QuickSetup.hpp"


namespace dx {
    void DX12QuickSetup::Init(HWND window, uint32_t width, uint32_t height)
    {  
        
        // enable debug layer
        mDX12Base.EnableDebugLayer();

        // query the gpu adapters
        mAdapter = mDX12Base.QueryDx12Adapters(false);

        // create a dx12 device
        mDevice = mDX12Base.CreateDevice(mAdapter);

        // create a command queue
        mCommandQueue = mDX12Base.CreateCommandQueue(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

        // create a swapchain
        mSwapChain = mDX12Base.CreateSwapChain(window, mCommandQueue, width, height, sNumFrames);

        // create a command allocator & command list 
        mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

        mRTVDescriptorHeap = mDX12Base.CreateDescriptorHeap(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, sNumFrames);
        mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        mDX12Base.UpdateRenderTargetViews(mDevice, mSwapChain, mRTVDescriptorHeap, mBackBuffers, sNumFrames);


        for(uint32_t i = 0; i < sNumFrames; i++)
        {
            mCommandAllocators[i] = mDX12Base.CreateCommandAllocator(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
        }

        mCommandList = mDX12Base.CreateCommandList(mDevice, mCommandAllocators[mCurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

        mFence = mDX12Base.CreateFence(mDevice);
        mFenceEvent = mDX12Base.CreateEventHandle();

    }
}  // namespace dx