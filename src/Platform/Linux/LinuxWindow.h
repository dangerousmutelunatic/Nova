#pragma once

#include "Core/Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Nova {

    class LinuxWindow : public Window {
    public:
        LinuxWindow(const WindowProps& props);
        virtual ~LinuxWindow() override;

        void OnUpdate() override;

        inline unsigned int GetWidth() const override { return data.Width; }
        inline unsigned int GetHeight() const override { return data.Height; }

        // Window attributes
        inline void SetEventCallback(const EventCallbackFn& callback) override { data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        void SetTitle(const std::string& title) override;
        void SetSize(int width, int height) override;

        bool IsKeyPressed(int key) override;
    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

    private:
        GLFWwindow* window;

        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData data;
    };

}