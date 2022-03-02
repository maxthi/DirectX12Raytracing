#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <queue>

using namespace Microsoft::WRL;

namespace dx {

    class CommandQueue
    {

      public:
        CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        virtual ~CommandQueue() = default;
        ComPtr<ID3D12GraphicsCommandList2> GetCommandList();

        /// @brief Execute a command list.
        /// @param commandList - The command list to execute.
        /// @return The the fence value to wait for for this command list.
        uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList);

        uint64_t                   Signal();
        bool                       IsFenceComplete(uint64_t fenceValue);
        void                       WaitForFenceValue(uint64_t fenceValue);
        void                       Flush();
        ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

      protected:
        ComPtr<ID3D12CommandAllocator>     CreateCommandAllocator();
        ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ComPtr<ID3D12CommandAllocator> allocator);

        // Keep track of command allocators that are "in-flight"
        struct CommandAllocatorEntry
        {
            uint64_t                       fenceValue;
            ComPtr<ID3D12CommandAllocator> commandAllocator;
        };

        using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
        using CommandListQueue      = std::queue<ComPtr<ID3D12GraphicsCommandList2>>;

        D3D12_COMMAND_LIST_TYPE    mCommandListType;
        ComPtr<ID3D12CommandQueue> mD3d12CommandQueue;
        ComPtr<ID3D12Fence>        mD3d12Fence;
        ComPtr<ID3D12Device2>      mD3d12Device;
        HANDLE                     mFenceEvent;
        uint64_t                   mFenceValue;

        CommandAllocatorQueue mCommandAllocatorQueue;
        CommandListQueue      mCommandListQueue;
    };
}  // namespace dx