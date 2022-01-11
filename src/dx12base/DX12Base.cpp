#include "DX12Base.hpp"

namespace dx {

    void DX12Base::Init()
    {
        // enable debug layer
        EnableDebugLayer();

        // query the gpu adapters
        auto adapter = QueryDx12Adapters(false);

        // create a dx12 device
        auto device = CreateDevice(adapter);

        // create a command queue
        // create a swapchain
        // create a command allocator & command list
    }

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
            NewFilter.DenyList.NumIDs  = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;
            ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
        }

#endif
        return d3d12Device2;
    }
}  // namespace dx
