#pragma once

#include "Core/Events/Event.h"
#include "Core/Core.h"

namespace Nova {

    struct WindowProps {
        std::string Title;
        unsigned int Width;
        unsigned int Height;

        WindowProps(const std::string& title = "Nova Application",
                    unsigned int width = 100,
                    unsigned int height = 100)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() {}

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        // User Window API
        virtual void SetTitle(const std::string& title) = 0;
        virtual void SetSize(const int width, const int height) = 0;
        virtual bool IsKeyPressed(int key) = 0;

        unsigned int width, height;

        static Ref<Window> Create(const WindowProps& props = WindowProps());
    };

}