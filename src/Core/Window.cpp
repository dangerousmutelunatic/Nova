#include "Window.h"

#ifdef NOVA_PLATFORM_LINUX
  #include "Platform/Linux/LinuxWindow.h"
#endif

namespace Nova {
  Ref<Window> Window::Create(const WindowProps& props) {
    #ifdef NOVA_PLATFORM_LINUX
      return CreateRef<LinuxWindow>(props);
    #else
     NOVA_ASSERT(false, "Unknown Platform")
      return nullptr;
    #endif
  }
}