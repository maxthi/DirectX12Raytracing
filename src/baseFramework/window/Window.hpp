#pragma once

#include <functional>

namespace base {
    class Window
    {
      public:
        Window(){};
        virtual ~Window(){};

        virtual void SetFullscreen(bool fullscreen){};

        /// @brief Give the control of the program flow to the window update loop.
        virtual void RunWindowloop(){};

        /// @brief Set the update callback, which is to be called by the window implementation each frame.
        void SetRenderUpdateCallback(std::function<void()> renderUpdateCallback) { mOnRenderUpdateCallback = renderUpdateCallback; };

        void SetKeyDownCallback(std::function<void(uint32_t)> keyDownCallback) { mOnKeyDownCallback = keyDownCallback; };

        void SetResizeCallback(std::function<void(uint32_t, uint32_t)> resizeCallback) { mOnResizeCallback = resizeCallback; };

      protected:
        bool mIsFullScreen{false};

        /// @brief !IMPORTANT! The render update callback function is to be called each frame in the
        /// render loop of the corresponding window implementation.
        std::function<void()> mOnRenderUpdateCallback;

        /// @brief The key down callback is set by the user application and called by the window implementation on each key down event.
        std::function<void(int32_t)> mOnKeyDownCallback;

        std::function<void(uint32_t, uint32_t)> mOnResizeCallback;
    };
}  // namespace base