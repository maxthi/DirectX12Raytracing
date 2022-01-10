// main file creates a win32 window and then proceeds the handle to the dx12 renderer
#include "main.hpp"

int main()
{
    // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
    // Using this awareness context allows the client area of the window
    // to achieve 100% scaling while still allowing non-client window content to
    // be rendered in a DPI sensitive fashion.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HWND window = CreateWin32Window();
    if(!window)
    {
        return ERROR_INVALID_WINDOW_HANDLE;
    }

    g_Renderer = std::make_unique<dx::Renderer>();
    g_Renderer->Init(window);

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

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
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_PAINT:
            g_Renderer->Update();
            break;
        case WM_KEYDOWN:
            if(wParam==VK_F11)
            {
                SetFullscreen(hwnd);
                break;
            }
            g_Renderer->KeyDown(wParam);
            break;
        case WM_SIZE: {
            RECT clientRect = {};
            GetClientRect(hwnd, &clientRect);
            int width  = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            g_Renderer->Resize(width, height);
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

HWND CreateWin32Window()
{
    // register window class
    LPCSTR     lpClassName = "DirectX12RaytracingClass";
    WNDCLASSEX wc;
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
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
    int       width               = 800;
    int       height              = 600;
    HWND      hWndParent          = nullptr;
    HMENU     hMenu               = nullptr;
    HINSTANCE hInstance           = nullptr;
    LPVOID    lpParam             = nullptr;

    // creating the window
    HWND window = CreateWindowEx(windowExtendedStyle, lpClassName, lpWindowName, windowStyle, xOrigin, yOrigin, width, height, hWndParent, hMenu, hInstance, lpParam);

    if(!window)
    {
        MessageBox(NULL, "Failed during window creation!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    return window;
}


void SetFullscreen(HWND window)
{
    static bool sFullscreen = false;
    sFullscreen             = !sFullscreen;

    // store original window settings
    static RECT windowRect;

    if(sFullscreen)
    {
        // Switching to fullscreen
        // Store the current window dimensions so they can be restored
        // when switching out of fullscreen state.
        GetWindowRect(window, &windowRect);

        // Set the window style to a borderless window so the client area fills
        // the entire screen.
        UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);


        SetWindowLongW(window, GWL_STYLE, windowStyle);

        // Query the name of the nearest display device for the window.
        // This is required to set the fullscreen dimensions of the window
        // when using a multi-monitor setup.
        HMONITOR      hMonitor    = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize        = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);
        SetWindowPos(window, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);
        ShowWindow(window, SW_MAXIMIZE);
    }
    else
    {
        // Restore all the window decorators.
        SetWindowLong(window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(window, HWND_NOTOPMOST, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                        SWP_FRAMECHANGED | SWP_NOACTIVATE);
        ShowWindow(window, SW_NORMAL);
    }
}