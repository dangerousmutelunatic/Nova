#pragma once

#include "Core/PlatformDetection.h"

#define NOVA_DEBUG

#ifdef NOVA_DEBUG
    #if defined(NOVA_PLATFORM_WINDOWS)
        #define NOVA_DEBUGBREAK() __debugbreak()
    #elif defined(NOVA_PLATFORM_LINUX)
        #include <csignal>
        #define NOVA_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
    #define NOVA_ENABLE_ASSERTS
#else
    #define NOVA_DEBUGBREAK()
#endif

#ifdef NOVA_ENABLE_ASSERTS
    #define NOVA_ASSERT(x, ...) { if(!(x)) { std::cout << std::format("NOVA: Assertion Failed: {0}", __VA_ARGS__) << std::endl; NOVA_DEBUGBREAK(); } }
#else
    #define NOVA_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Nova {
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}