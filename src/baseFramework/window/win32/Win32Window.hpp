#include "../Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>

namespace base {

    class Win32Window : public Window
    {
      public:
        static LRESULT CALLBACK          StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static std::vector<Win32Window*> sWindowList;

      public:
        Win32Window();
        virtual ~Win32Window();

        HWND             CreateWin32Window(uint32_t width, uint32_t height);
        HWND             GetWindow() { return mWindowHandle; };
        LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        virtual void     SetFullscreen(bool fullscreen) override;

        /// @brief Give the control of the program flow to the window update loop.
        virtual void RunWindowloop() override;

      protected:
        HWND mWindowHandle{nullptr};
    };
}  // namespace base