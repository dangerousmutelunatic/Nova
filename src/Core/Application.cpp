#include "Application.h"

//#include <glad/glad.h>

#include "Events/MouseEvent.h"
#include "KeyCodes.h"

namespace Nova {

    void Application::Initialize() {
        window = Window::Create();
        window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        keyboard.setWindow(window);

        mouse.x = 0;
        mouse.y = 0;
        mouse.px = 0;
        mouse.py = 0;
        mouse.dx = 0;
        mouse.dy = 0;
        mouse.wheelX = 0;
        mouse.wheelY = 0;
        mouse.buttonsDown = 0;
        mouse.button = 0;

        Setup();
    }

    void Application::Run() {
        while (running)
        {
            //glClearColor(1, 0, 1, 1);
            //glClear(GL_COLOR_BUFFER_BIT);
            window->OnUpdate();
            Update();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(OnKeyReleased));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMousePressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseReleased));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

        if (!e.Handled) {
            std::cout << "Unhandled event: " << e << std::endl;
        }
    }

    bool Application::OnKeyPressed(KeyPressedEvent &e) {
        if (e.GetRepeatCount()) {
            KeyRepeat();
        } else {
            keyboard.keyCode = e.GetKeyCode();
            if (keyboard.keyCode == KEY_LEFT_SHIFT || keyboard.keyCode == KEY_RIGHT_SHIFT) { keyboard.shiftHeld = true; }
            else if (keyboard.keyCode == KEY_LEFT_CONTROL || keyboard.keyCode == KEY_RIGHT_CONTROL) { keyboard.ctrlHeld = true; }
            else if (keyboard.keyCode == KEY_LEFT_ALT || keyboard.keyCode == KEY_RIGHT_ALT) { keyboard.altHeld = true; }

            keyboard.key = (keyboard.keyCode >= KEY_SPACE && keyboard.keyCode <= KEY_GRAVE_ACCENT) ? keyboard.keyCode : 0;
            KeyPressed();
        }
        return true;
    }

    bool Application::OnKeyReleased(KeyReleasedEvent &e) {
        keyboard.keyCode = e.GetKeyCode();
        if (keyboard.keyCode == KEY_LEFT_SHIFT && !keyboard.isKeyDown(KEY_RIGHT_SHIFT)
            || keyboard.keyCode == KEY_RIGHT_SHIFT && !keyboard.isKeyDown(KEY_LEFT_SHIFT)) { keyboard.shiftHeld = false; }
        else if (keyboard.keyCode == KEY_LEFT_CONTROL && !keyboard.isKeyDown(KEY_RIGHT_CONTROL)
            || keyboard.keyCode == KEY_RIGHT_CONTROL && !keyboard.isKeyDown(KEY_LEFT_CONTROL)) { keyboard.ctrlHeld = false; }
        else if (keyboard.keyCode == KEY_LEFT_ALT && !keyboard.isKeyDown(KEY_RIGHT_ALT)
            || keyboard.keyCode == KEY_RIGHT_ALT && !keyboard.isKeyDown(KEY_LEFT_ALT)) { keyboard.altHeld = false; }

        keyboard.key = (keyboard.keyCode >= KEY_SPACE && keyboard.keyCode <= KEY_GRAVE_ACCENT) ? keyboard.keyCode : 0;
        KeyReleased();
        return true;
    }

    bool Application::OnMousePressed(MouseButtonPressedEvent &e) {
        mouse.button = e.GetMouseButton();
        mouse.buttonsDown |= (1 << mouse.button);
        MousePressed();
        return true;
    }

    bool Application::OnMouseReleased(MouseButtonReleasedEvent &e) {
        mouse.button = e.GetMouseButton();
        mouse.buttonsDown &= ~(1 << mouse.button);
        MouseReleased();
        return true;
    }

    bool Application::OnMouseMoved(MouseMovedEvent &e) {
        mouse.px = mouse.x;
        mouse.py = mouse.y;
        mouse.x = e.GetX();
        mouse.y = e.GetY();
        mouse.dx = mouse.x - mouse.px;
        mouse.dy = mouse.y - mouse.py;
        if (mouse.buttonsDown) {
            MouseDragged();
        } else {
            MouseMoved();
        }
        return true;
    }

    bool Application::OnMouseScrolled(MouseScrolledEvent& e) {
        mouse.wheelX = e.GetXOffset();
        mouse.wheelY = e.GetYOffset();
        MouseScrolled();
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e) {
        window->width = e.GetWidth();
        window->height = e.GetHeight();
        WindowResized();
        return true;
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        running = false;
        WindowCloseAttempt();
        return true;
    }

    //====================================================================================
    // Default Function Bodies
    //====================================================================================
    void Application::Setup() {}
    void Application::Update() {}
    void Application::KeyPressed() {}
    void Application::KeyReleased() {}
    void Application::KeyRepeat() {}
    void Application::MousePressed() {}
    void Application::MouseReleased() {}
    void Application::MouseDragged() {}
    void Application::MouseMoved() {}
    void Application::MouseScrolled() {}
    void Application::WindowResized() {}
    void Application::WindowCloseAttempt() {}
}
