#pragma once

#include "Core/Renderer.h"

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Nova {
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer(Ref<Window> w);
        ~OpenGLRenderer() override;

        void OnWindowResize(uint32_t width, uint32_t height) override;

        void rect(float x, float y, float w, float h, float r) override;
        void ellipse(float x, float y, float w, float h) override;

    private:
        static GLuint compileShader(GLenum type, const char* src);

    private:
        GLuint rectVAO = 0, rectVBO = 0, ellipseVAO = 0, ellipseVBO = 0;
        GLuint rectShader = 0, ellipseShader = 0;
    };
}