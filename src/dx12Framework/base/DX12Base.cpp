#include "DX12Base.hpp"
using namespace Microsoft::WRL;

namespace dx {

    //------------------------------------------------------------------------------------------------
    void DX12Base::EnableDebugLayer()
    {
#if defined(_DEBUG)
        // always enable the debug layer before doing anything dx12 related
        // so all possible errors generated while creating direct dx12 objects are caught by the debug layer
        ComPtr<ID3D12Debug> debugInterface;
        // The IID_PPV_ARGS marcro automatically computes the IID of based on the required interface
        // Always use for functions that require separate IID and interface pointer parameters
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
        debugInterface->EnableDebugLayer();
#endif
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<IDXGIAdapter4> DX12Base::QueryDx12Adapters(bool useWarp)
    {
        ComPtr<IDXGIFactory4> dxgiFactory;
        UINT                  createFactoryFlags = 0;
#if defined(_DEBUG)
        // enables errors caught during device creation
        // and while querying for for the adapters
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        ComPtr<IDXGIAdapter4> dxgiAdapter4;

        if(useWarp)
        {
            // create warp adapter
            ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
            // cast to adapter4 in order to match return type
            ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
        }
        else
        {
            size_t maxDedicatedVideoMemory = 0;
            for(UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
                dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

                // check to see if the adapter can create a d3d12 devie without actually
                // creating it. The adapter with the largest dedicated video memory is favored
                if((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0
                   && SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))
                   && dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
                {
                    maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                    ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
                }
            }
        }

        return dxgiAdapter4;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12Device2> DX12Base::CreateDevice(ComPtr<IDXGIAdapter4> adapter)
    {
        ComPtr<ID3D12Device2> d3d12Device2;
        ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

        // enable debug messages in debug mode
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> pInfoQueue;
        if(SUCCEEDED(d3d12Device2.As(&pInfoQueue)))  // is info queried or cast here??
        {
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);  // on memory corruption
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);       // all errors should be solved
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);     // not all warnings can always be solved

            //Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};

            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] = {D3D12_MESSAGE_SEVERITY_INFO};

            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,  // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                        // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                      // This warning occurs when using capture frame while graphics debugging.
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs        = _countof(DenyIds);
            NewFilter.DenyList.pIDList       = DenyIds;
            ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
        }

#endif
        return d3d12Device2;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12CommandQueue> DX12Base::CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type                     = type;
        desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask                 = 0;

        ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));
        return d3d12CommandQueue;
    }

    //------------------------------------------------------------------------------------------------
    bool DX12Base::CheckTearingSupport()
    {
        // variable resfresh rate displays (NVidias G-Sync, AMD's Free Sync) require tearing
        // to be enabled to function
        bool                  allowTearing = false;
        ComPtr<IDXGIFactory4> factory4;
        if(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
        {
            ComPtr<IDXGIFactory5> factory5;
            if(SUCCEEDED(factory4.As(&factory5)))
            {
                if(FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
                {
                    allowTearing = false;
                }
            }
        }

        return allowTearing;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<IDXGISwapChain4> DX12Base::CreateSwapChain(HWND hWnd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
    {
        ComPtr<IDXGISwapChain4> dxgiSwapChain4;
        ComPtr<IDXGIFactory4>   dxgiFactory4;
        UINT                    createFactoryFlags = 0;
#if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

        // DXGI WARNING: IDXGIFactory::CreateSwapChain: DXGI_SWAP_CHAIN_DESC.OutputWindow is not a valid window handle.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width                 = width;
        swapChainDesc.Height                = height;
        swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo                = false;
        swapChainDesc.SampleDesc            = {1, 0};
        swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount           = bufferCount;
        swapChainDesc.Scaling               = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags                 = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;


        ComPtr<IDXGISwapChain1> swapChain1;

        // create actualy swapchain
        ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1));

        // remove alt enter functionality
        ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

        // cast to swapchain4
        ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

        return dxgiSwapChain4;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12DescriptorHeap> DX12Base::CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors             = numDescriptors;
        desc.Type                       = type;

        ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
        return descriptorHeap;
    }

    //------------------------------------------------------------------------------------------------
    void DX12Base::UpdateRenderTargetViews(ComPtr<ID3D12Device2> device, ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap, ComPtr<ID3D12Resource>* backBuffers, uint32_t bufferCount)
    {

        // the size of a single descriptor is vendor specific
        auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // get a handle to first descriptor in the heap in order to iterate them
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

        for(int i = 0; i < bufferCount; ++i)
        {
            ComPtr<ID3D12Resource> backBuffer;
            ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

            // first parameter: pointer to destination resource
            // second: rtv description, if nullptr take resources default
            // third: location where the resource is placed
            device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

            backBuffers[i] = backBuffer;

            // offset to the next handle
            rtvHandle.Offset(rtvDescriptorSize);
        }
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12CommandAllocator> DX12Base::CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
    {
        // a command allocator can only be used by a single command list at a time
        // but can be reused after all commands that were recorded into the command list
        // have finished executing on the gpu
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));

        // memory allocated by a commandAllocator can be reclaimed with ID3D12CommandAllocator::Reset, but only
        // after the recorded commands have finished executing on the GPU

        // to check this, use a GPU fence for example
        return commandAllocator;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12GraphicsCommandList> DX12Base::CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
    {
        // command list can be immediately resued after it has been executed on the command queue
        // but it must be reset before recording new commands
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

        // before a commandlist can be reset it must be closed
        ThrowIfFailed(commandList->Close());

        return commandList;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12Fence> DX12Base::CreateFence(ComPtr<ID3D12Device2> device)
    {
        // a fence can be user signaled from the CPU or the GPU
        // CPU: ID3D12Fence::Signal
        // GPU: ID3D12CommandQueue::Signal
        // to wait for a fence
        // CPU: ID3D12Fence::SetEventOnCompletion & WaitForSingleObject
        // GPU: ID3D12CommandQueue::Wait
        ComPtr<ID3D12Fence> fence;
        ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        return fence;
    }

    //------------------------------------------------------------------------------------------------
    HANDLE DX12Base::CreateEventHandle()
    {
        // a event blocks CPU processing until the fence has been signaled
        HANDLE fenceEvent;
        fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(fenceEvent && "Failed to create fence event!");
        return fenceEvent;
    }

    //------------------------------------------------------------------------------------------------
    uint64_t DX12Base::Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue)
    {

        // decide which value should be next in the fence
        uint64_t fenceValueForSignal = ++fenceValue;
        ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValueForSignal));

        return fenceValueForSignal;
    }

    //------------------------------------------------------------------------------------------------
    void DX12Base::WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration)
    {
        // stall the CPU if we need to wait for the GPU to free a resource
        if(fence->GetCompletedValue() < fenceValue)
        {
            ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
            WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
        }
    }


    //------------------------------------------------------------------------------------------------
    void DX12Base::Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent)
    {
        // make sure all commands previously executed on the gpu have finished
        // for example to make sure backbuffer resources are not referenced anymore
        // also flush GPU before releasing any resources that might be referenced
        uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
        WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
    }

}  // namespace dx
