#pragma once

#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer.h"

namespace Nova {

    struct Keyboard {
        int keyCode;
        char key;
        bool shiftHeld, ctrlHeld, altHeld;

        void setWindow(const Ref<Window>& w) { window = w; }

        bool isKeyDown(int key) const { return window->IsKeyPressed(key); }

    private:
        Ref<Window> window;
    };

    struct Mouse {
        float x, y, px, py, dx, dy, wheelX, wheelY;
        uint8_t button, buttonsDown;
    };

    class Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Initialize();
        void Run();

        virtual void Setup();
        virtual void Update();
        virtual void KeyPressed();
        virtual void KeyReleased();
        virtual void KeyRepeat();
        virtual void MousePressed();
        virtual void MouseReleased();
        virtual void MouseDragged();
        virtual void MouseMoved();
        virtual void MouseScrolled();
        virtual void WindowResized();
        virtual void WindowCloseAttempt();

    protected:
        void OnEvent(Event& e);

        bool OnKeyPressed(KeyPressedEvent& e); // Used to generate KeyPressed and KeyRepeated events
        bool OnKeyReleased(KeyReleasedEvent& e);
        bool OnMousePressed(MouseButtonPressedEvent& e);
        bool OnMouseReleased(MouseButtonReleasedEvent& e);
        bool OnMouseMoved(MouseMovedEvent& e); // Used to generate MouseDragged events
        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        bool OnWindowClose(WindowCloseEvent& e);

        Mouse mouse;
        Keyboard keyboard;

        Ref<Window> window;
        bool running = true;

        Ref<Renderer> renderer;
    };

}
