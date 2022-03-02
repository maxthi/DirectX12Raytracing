#include "Win32Window.hpp"

namespace base {

    // static window list
    std::vector<Win32Window*> Win32Window::sWindowList = {};

    Win32Window::Win32Window() { 
        sWindowList.push_back(this);
        // create default callbacks
        mOnResizeCallback = [](uint32_t, uint32_t) {};
        mOnKeyDownCallback = [](uint32_t) {};
        mOnRenderUpdateCallback = []() {};
    }
    Win32Window::~Win32Window() { 
        auto position = std::find(sWindowList.begin(), sWindowList.end(), this);
        if(position != sWindowList.end())  // == .end() means the element was not found
            sWindowList.erase(position);
    }

    LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
            case WM_PAINT:
                mOnRenderUpdateCallback();
                break;
            case WM_KEYDOWN:
                mOnKeyDownCallback(wParam);
                break;
            case WM_SIZE: {
                RECT clientRect = {};
                GetClientRect(hwnd, &clientRect);
                int width  = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                mOnResizeCallback(width, height);
            }
            break;
            case WM_SYSCHAR:
                break;
            case WM_CLOSE:
                DestroyWindow(hwnd);
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
            case WM_PAINT:
                //mOnRenderUpdateCallback();
                break;
            case WM_KEYDOWN:
                //mOnKeyDownCallback(wParam);
                break;
            case WM_SIZE: {
                RECT clientRect = {};
                GetClientRect(hwnd, &clientRect);
                int width  = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                //mOnResizeCallback(width, height);
            }
            break;
            case WM_SYSCHAR:
                break;
            case WM_CLOSE:
                DestroyWindow(hwnd);
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    LRESULT CALLBACK Win32Window::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
            case WM_PAINT:
            case WM_KEYDOWN:
            case WM_SIZE:
            case WM_SYSCHAR:
            case WM_CLOSE:
            case WM_DESTROY:
                for(auto windowPtr : sWindowList)
                {
                    windowPtr->WndProc(hwnd, msg, wParam, lParam);
                }
                break;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }

    }

    HWND Win32Window::CreateWin32Window(uint32_t width, uint32_t height)
    {
        // register window class
        LPCSTR     lpClassName = "DirectX12RaytracingClass";
        WNDCLASSEX wc;
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = 0;
        wc.lpfnWndProc   = StaticWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = nullptr;
        wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName  = nullptr;
        wc.lpszClassName = lpClassName;
        wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

        if(!RegisterClassEx(&wc))
        {
            MessageBox(NULL, "Failed during window class registration!", "Error!", MB_ICONEXCLAMATION | MB_OK);
            return 0;
        }

        // window creation parameters
        LPCSTR    lpWindowName        = "DirectX12Raytracing";
        DWORD     windowExtendedStyle = WS_EX_CLIENTEDGE;
        DWORD     windowStyle         = WS_OVERLAPPEDWINDOW;
        int       xOrigin             = CW_USEDEFAULT;
        int       yOrigin             = CW_USEDEFAULT;
        HWND      hWndParent          = nullptr;
        HMENU     hMenu               = nullptr;
        HINSTANCE hInstance           = nullptr;
        LPVOID    lpParam             = nullptr;

        // creating the window
        mWindowHandle = CreateWindowEx(windowExtendedStyle, lpClassName, lpWindowName, windowStyle, xOrigin, yOrigin, width, height, hWndParent, hMenu, hInstance, lpParam);

        if(!mWindowHandle)
        {
            MessageBox(NULL, "Failed during window creation!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        }

        return mWindowHandle;
    }

    void Win32Window::SetFullscreen(bool fullscreen)
    {
        // store original window settings
        static RECT windowRect;

        // break if fullscreen state is unchanged
        if(mIsFullScreen == fullscreen)
            return;

        // update fullscreen state
        mIsFullScreen = fullscreen;

        if(mIsFullScreen)
        {
            // Switching to fullscreen
            // Store the current window dimensions so they can be restored
            // when switching out of fullscreen state.
            GetWindowRect(mWindowHandle, &windowRect);

            // Set the window style to a borderless window so the client area fills
            // the entire screen.
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);


            SetWindowLongW(mWindowHandle, GWL_STYLE, windowStyle);

            // Query the name of the nearest display device for the window.
            // This is required to set the fullscreen dimensions of the window
            // when using a multi-monitor setup.
            HMONITOR      hMonitor    = ::MonitorFromWindow(mWindowHandle, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize        = sizeof(MONITORINFOEX);
            GetMonitorInfo(hMonitor, &monitorInfo);
            SetWindowPos(mWindowHandle, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                         monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);
            ShowWindow(mWindowHandle, SW_MAXIMIZE);
        }
        else
        {
            // Restore all the window decorators.
            SetWindowLong(mWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(mWindowHandle, HWND_NOTOPMOST, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                         SWP_FRAMECHANGED | SWP_NOACTIVATE);
            ShowWindow(mWindowHandle, SW_NORMAL);
        }
    }

    void Win32Window::RunWindowloop()
    {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        ShowWindow(mWindowHandle, SW_SHOW);
        UpdateWindow(mWindowHandle);

        // win32 window update loop
        MSG msg = {};
        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

}  // namespace base