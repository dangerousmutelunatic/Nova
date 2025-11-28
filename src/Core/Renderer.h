#pragma once

#include "Window.h"
#include "Core/Core.h"
#include "Core/Color.h"

namespace Nova {

    class Renderer {
    public:
        Renderer(Ref<Window> w);
        virtual ~Renderer() = default;

        virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

        template <typename... Args>
        void background(Args&&... args) {
            Color::RGBA tmpC = fillColor;
            int tmp = m_strokeWeight;
            fillColor = Color::RGBA(std::forward<Args>(args)...);
            m_strokeWeight = 0;
            rect(0, 0, windowWidth, windowHeight);
            m_strokeWeight = tmp;
            fillColor = tmpC;
        }

        template <typename... Args>
        void stroke(Args&&... args) {
            strokeColor = Color::RGBA(std::forward<Args>(args)...);
        }
        void noStroke();
        void strokeWeight(int w);

        template <typename... Args>
        void fill(Args&&... args) {
            fillColor = Color::RGBA(std::forward<Args>(args)...);
        }
        void noFill();

        virtual void rect(float x, float y, float w, float h, float r = 0.0f) = 0;
        virtual void ellipse(float x, float y, float w, float h) = 0;

        static Ref<Renderer> Create(Ref<Window> w);
    protected:
        int windowWidth = 0, windowHeight = 0;
        Ref<Window> window;

        Color::RGBA strokeColor, fillColor;
        int m_strokeWeight;
    };


}