#include "CommandQueue.hpp"
#include "DX12Includes.hpp"

namespace dx {
    //------------------------------------------------------------------------------------------------
    CommandQueue::CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) : mFenceValue(0), mD3d12Device(device), mCommandListType(type)
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type                     = type;
        desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask                 = 0;

        ThrowIfFailed(mD3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&mD3d12CommandQueue)));
        ThrowIfFailed(mD3d12Device->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(% mD3d12Fence)));

        mFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(mFenceEvent && "Failed to create fence event handle");
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
    {

        // a command allocator can be reused if the commands finished executing on the GPU
        ComPtr<ID3D12CommandAllocator> commandAllocator;

        // a command list uses a command allocator & can be reused as soon it was submitted to the GPU (aka excuted).
        ComPtr<ID3D12GraphicsCommandList> commandList;

        // if there is at least one element in the queue AND the frontelements fence has completed
        // then remove the element from the queue
        if(!mCommandAllocatorQueue.empty() && IsFenceComplete(mCommandAllocatorQueue.front().fenceValue))
        {
            commandAllocator = mCommandAllocatorQueue.front().commandAllocator;
            mCommandAllocatorQueue.pop();
            ThrowIfFailed(commandAllocator->Reset());
        }
        else
        {
            commandAllocator = CreateCommandAllocator();
        }

        if(!mCommandListQueue.empty())
        {
            commandList = mCommandListQueue.front();
            mCommandListQueue.pop();
            ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
        }
        else
        {
            commandList = CreateCommandList(commandAllocator);
        }

        // Associate the command allocator with the comand list
        // so it can be retrieved when command list is executed.
        ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

        return commandList;
    }

    //------------------------------------------------------------------------------------------------
    uint64_t CommandQueue::ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
    {
        // close the command list so it can be reset
        commandList->Close();

        // get the associated cmd memory - aka allocator
        ID3D12CommandAllocator* commandAllocator;
        UINT                    dataSize = sizeof(commandAllocator);
        ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

        // wrap the command list to process it with ExecuteCommandLists
        ID3D12CommandList* const ppCommandLists[] = {commandList.Get()};
        mD3d12CommandQueue->ExecuteCommandLists(1, ppCommandLists);
        uint64_t fenceValue = Signal();

        // store the
        mCommandAllocatorQueue.emplace(CommandAllocatorEntry{fenceValue, commandAllocator});
        mCommandListQueue.push(commandList);

        commandAllocator->Release();
        return fenceValue;
    }

    //------------------------------------------------------------------------------------------------
    uint64_t CommandQueue::Signal()
    {
        uint64_t fenceValueForSignal = ++mFenceValue;
        ThrowIfFailed(mD3d12CommandQueue->Signal(fence.Get(), fenceValueForSignal));

        return fenceValueForSignal;
    }

    //------------------------------------------------------------------------------------------------
    bool CommandQueue::IsFenceComplete(uint64_t fenceValue) { return mD3d12Fence->GetCompletedValue() >= fenceValue; }

    //------------------------------------------------------------------------------------------------
    void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
    {
        if(!IsFenceComplete(fenceValue))
        {
            mD3d12Fence->SetEventOnCompletion(fenceValue, mFenceEvent);
            WaitForSingleObject(mFenceEvent, DWORD_MAX);
        }
    }

    //------------------------------------------------------------------------------------------------
    void CommandQueue::Flush()
    {
        uint64_t fenceValueForSignal = Signal();
        WaitForFenceValue(fenceValueForSignal);
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const { return mD3d12CommandQueue; }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ThrowIfFailed(mD3dDevice->CreateCommandAllocator(mCommandListType, IID_PPV_ARGS(&commandAllocator)));
        return commandAllocator;
    }

    //------------------------------------------------------------------------------------------------
    ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(ComPtr<ID3D12CommandAllocator> allocator)
    {
        ComPtr<ID3D12GraphicsCommandList2> commandList;
        ThrowIfFailed(mD3d12Device->CreateCommandList(0, mCommandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        return commandList;
    }
}  // namespace dx