#include "Renderer.h"

#include "glm/vec4.hpp"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Nova {
    Renderer::Renderer(Ref<Window> w)
        : window(w), strokeColor(Color::Black), fillColor(Color::White), m_strokeWeight(1){
    }

    void Renderer::noStroke() {
        strokeColor = Color::RGBA(0,0,0,0);
    }

    void Renderer::strokeWeight(int w) {
        m_strokeWeight = w;
    }

    void Renderer::noFill() {
        fillColor = Color::RGBA(0,0,0,0);
    }

    Ref<Renderer> Renderer::Create(Ref<Window> w) {
        #ifdef NOVA_PLATFORM_OPENGL
            return CreateRef<OpenGLRenderer>(w);
        #else
            NOVA_ASSERT(false, "Unknown Renderer Platform!")
            return nullptr;
        #endif
    }
}