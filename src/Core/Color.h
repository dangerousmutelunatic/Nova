#pragma once

#include <cstdint>

namespace Nova::Color {

    struct RGB {
        std::uint8_t r, g, b;

        constexpr RGB(int r, int g, int b)
        : r(limit(r)), g(limit(g)), b(limit(b)) {}

        constexpr RGB(int v)
        : RGB(v, v, v) {}

        constexpr RGB(std::uint32_t hex)
        : r((hex >> 16) & 0xFF),
          g((hex >> 8) & 0xFF),
          b(hex & 0xFF) {}

    private:
        static constexpr std::uint8_t limit(int v) {
            return v % 256;
        }
    };

    struct HSV {
        std::uint8_t h, s, v;

        constexpr HSV(int h, int s, int v)
        : h(limit(h)), s(limit(s)), v(limit(v)) {}

        constexpr HSV(std::uint32_t hex)
        : h((hex >> 16) & 0xFF),
          s((hex >> 8) & 0xFF),
          v(hex & 0xFF) {}

    private:
        static constexpr std::uint8_t limit(int v) {
            return v % 256;
        }
    };

    struct RGBA {
        std::uint8_t r, g, b, a;

        constexpr RGBA(int r, int g, int b, int a)
        : r(limit(r)), g(limit(g)), b(limit(b)), a(limit(a)) {}

        constexpr RGBA(int r, int g, int b)
        : RGBA(r, g, b, 255) {}

        constexpr RGBA(int v)
        : RGBA(v, v, v, 255) {}

        constexpr RGBA(std::uint32_t hex)
        : r((hex >> 24) & 0xFF),
          g((hex >> 16) & 0xFF),
          b((hex >> 8) & 0xFF),
          a(hex & 0xFF) {}

        constexpr RGBA(RGB rgb)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(255) {}

    private:
        static constexpr std::uint8_t limit(int v) {
            return v % 256;
        }
    };

    constexpr RGBA Red = {255, 0, 0};
    constexpr RGBA Green = {0, 255, 0};
    constexpr RGBA Blue = {0, 0, 255};
    constexpr RGBA White = {255, 255, 255};
    constexpr RGBA Black = {0, 0, 0};
    constexpr RGBA Yellow = {255, 255, 0};
    constexpr RGBA Magenta = {255, 0, 255};
    constexpr RGBA Cyan = {0, 255, 255};
    constexpr RGBA TardisBlue = 0x003b6fff;

}