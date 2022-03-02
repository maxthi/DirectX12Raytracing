#include "../../baseFramework/window/win32/Win32Window.hpp"
#include "../../dx12Framework/base/DX12QuickSetup.hpp"


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch(message)
    {
        case WM_PAINT:

            break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN: {
            bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            switch(wParam)
            {
                case 'V':

                    break;
                case VK_ESCAPE:
                    ::PostQuitMessage(0);
                    break;
                case VK_RETURN:
                    break;
            }
        }
        break;
        // The default window procedure will play a system notification sound
        // when pressing the Alt+Enter keyboard combination if this message is
        // not handled.
        case WM_SYSCHAR:
            break;
        case WM_SIZE: {
            RECT clientRect = {};


            int width  = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
        }
        break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;
        default:
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}

void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName)
{
    // Register a window class for creating our render window with.
    WNDCLASSEXW windowClass = {};

    windowClass.cbSize        = sizeof(WNDCLASSEXW);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = &WndProc;
    windowClass.cbClsExtra    = 0;
    windowClass.cbWndExtra    = 0;
    windowClass.hInstance     = hInst;
    windowClass.hIcon         = ::LoadIcon(hInst, NULL);  //  MAKEINTRESOURCE(APPLICATION_ICON));
    windowClass.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszMenuName  = NULL;
    windowClass.lpszClassName = windowClassName;
    windowClass.hIconSm       = ::LoadIcon(hInst, NULL);  //  MAKEINTRESOURCE(APPLICATION_ICON));

    static HRESULT hr = ::RegisterClassExW(&windowClass);
    assert(SUCCEEDED(hr));
}

HWND CreateWindow2(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, uint32_t width, uint32_t height)
{
    int screenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    int windowWidth  = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Center the window within the screen. Clamp to 0, 0 for the top-left corner.
    int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
    int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

    HWND hWnd = ::CreateWindowExW(NULL, windowClassName, windowTitle, WS_OVERLAPPEDWINDOW, windowX, windowY, windowWidth, windowHeight, NULL, NULL, hInst, nullptr);

    assert(hWnd && "Failed to create window");

    return hWnd;
}

class SimpleTriangleRendering
{
  protected:
    dx::DX12QuickSetup mQuickSetup;
    bool               mVSync            = true;
    bool               mTearingSupported = false;

  public:
    SimpleTriangleRendering()
    {
        base::Win32Window window;
        window.SetKeyDownCallback([this](uint32_t keycode) { OnKeyDown(keycode); });

        uint32_t width       = 1500;
        uint32_t height      = 600;
        HWND     win32Window = window.CreateWin32Window(width, height);
        window.SetRenderUpdateCallback([this]() { Update(); });
        mQuickSetup.Init(win32Window, width, height);

        mTearingSupported = mQuickSetup.mDX12Base.CheckTearingSupport();

        window.RunWindowloop();
    }

    bool LoadContent() {

    }

    void UnloadContent() {

    }

    void Update()
    {
        static uint64_t                           frameCounter   = 0;
        static double                             elapsedSeconds = 0.0;
        static std::chrono::high_resolution_clock clock;
        static auto                               t0 = clock.now();

        frameCounter++;
        auto t1        = clock.now();
        auto deltaTime = t1 - t0;
        t0             = t1;

        // convert from nanoseconds to seconds
        elapsedSeconds += deltaTime.count() * 1e-9;

        if(elapsedSeconds > 1.0)
        {
            char buffer[500];
            auto fps = frameCounter / elapsedSeconds;
            sprintf_s(buffer, 500, "FPS: %f\n", fps);
            OutputDebugString(buffer);

            frameCounter   = 0;
            elapsedSeconds = 0.0;
        }

        // todo: create a framework library, that can basically do all that stuff for you and you can use for setting up projects

        auto commandAllocator = mQuickSetup.mCommandAllocators[mQuickSetup.mCurrentBackBufferIndex];
        auto backBuffer       = mQuickSetup.mBackBuffers[mQuickSetup.mCurrentBackBufferIndex];

        commandAllocator->Reset();
        mQuickSetup.mCommandList->Reset(commandAllocator.Get(), nullptr);

        // Clear the render target.
        {
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

            mQuickSetup.mCommandList->ResourceBarrier(1, &barrier);
            FLOAT                         clearColor[] = {0.4f, 0.6f, 0.9f, 1.0f};
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(mQuickSetup.mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), mQuickSetup.mCurrentBackBufferIndex,
                                              mQuickSetup.mRTVDescriptorSize);

            mQuickSetup.mCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
        }

        // Present
        {
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            mQuickSetup.mCommandList->ResourceBarrier(1, &barrier);
            ThrowIfFailed(mQuickSetup.mCommandList->Close());

            ID3D12CommandList* const commandLists[] = {mQuickSetup.mCommandList.Get()};
            mQuickSetup.mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
            UINT syncInterval = mVSync ? 1 : 0;
            UINT presentFlags = mTearingSupported && !mVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
            ThrowIfFailed(mQuickSetup.mSwapChain->Present(syncInterval, presentFlags));

            mQuickSetup.mFrameFenceValues[mQuickSetup.mCurrentBackBufferIndex] =
                mQuickSetup.mDX12Base.Signal(mQuickSetup.mCommandQueue, mQuickSetup.mFence, mQuickSetup.mFenceValue);

            mQuickSetup.mCurrentBackBufferIndex = mQuickSetup.mSwapChain->GetCurrentBackBufferIndex();

            mQuickSetup.mDX12Base.WaitForFenceValue(mQuickSetup.mFence, mQuickSetup.mFrameFenceValues[mQuickSetup.mCurrentBackBufferIndex], mQuickSetup.mFenceEvent);
        }
    }

    void OnKeyDown(uint32_t keycode){};
};

int main()
{
    SimpleTriangleRendering r;
}