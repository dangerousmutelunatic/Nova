#include "LinuxWindow.h"

#include "Core/Core.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/KeyEvent.h"

#include <glad/glad.h>

namespace Nova {
	
    static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description) {
		std::cout << std::format("GLFW Error ({}): {}", error, description) << std::endl;
	}

    LinuxWindow::LinuxWindow(const WindowProps& props) {
        LinuxWindow::Init(props);
    }

    LinuxWindow::~LinuxWindow() {
        LinuxWindow::Shutdown();
    }

    void LinuxWindow::Init(const WindowProps& props) {
        data.Title = props.Title;
        data.Width = props.Width;
        data.Height = props.Height;
    	width = props.Width;
    	height = props.Height;

        if (!s_GLFWInitialized)
        {
            // TODO: glfwTerminate on system shutdown
            int success = glfwInit();
            NOVA_ASSERT(success, "Could not initialize GLFW!");
        	glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), data.Title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    	NOVA_ASSERT(status, "Could not initialize GLAD!");
    	glfwSetWindowUserPointer(window, &data);
        SetVSync(true);

        // Set GLFW callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
    }

    void LinuxWindow::Shutdown() {
        glfwDestroyWindow(window);
    }

    void LinuxWindow::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    void LinuxWindow::SetVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        data.VSync = enabled;
    }

    bool LinuxWindow::IsVSync() const {
        return data.VSync;
    }

    void LinuxWindow::SetTitle(const std::string& title) {
    	glfwSetWindowTitle(window, title.c_str());
    }

    void LinuxWindow::SetSize(const int width, const int height) {
    	glfwSetWindowSize(window, width, height);
    	this->width = width;
    	this->height = height;
    }

    bool LinuxWindow::IsKeyPressed(int key) {
		return glfwGetKey(window, key) == GLFW_PRESS;
    }
}
