#include "OpenGLRenderer.h"

namespace Nova {
    OpenGLRenderer::OpenGLRenderer(Ref<Window> w) : Renderer(w) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        NOVA_ASSERT(rectVAO == 0, "rectVAO already Initialized!");
        NOVA_ASSERT(rectVAO == 0, "ellipseVAO already Initialized!");

        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        glGenVertexArrays(1, &ellipseVAO);
        glGenBuffers(1, &ellipseVBO);

        // --- Vertex Shader ---
        const char* vsSrc = R"(
            #version 420 core
            layout(location = 0) in vec2 pos;

            uniform vec2 screenSize;

            out vec2 pixelCoord;

            void main() {
                pixelCoord = pos;  // pass pixel coords directly to fragment shader

                float ndcX =  (pos.x / screenSize.x) * 2.0 - 1.0;
                float ndcY = -(pos.y / screenSize.y) * 2.0 + 1.0;

                gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);
            }
        )";

        // --- Fragment Shader ---
        const char* fsSrc = R"(
            #version 420 core
            in vec2 pixelCoord;

            out vec4 fragColor;

            uniform vec2 rectPos;
            uniform vec2 rectSize;
            uniform float radius;

            uniform vec4 strokeColor;
            uniform vec4 fillColor;

            uniform float strokeWeight;

            // signed distance to rounded rect
            float roundedRectSDF(vec2 p, vec2 size, float rad) {
                vec2 d = abs(p - size*0.5) - (size*0.5 - rad);
                return length(max(d, 0.0)) - rad;
            }

            void main() {
                vec2 local = pixelCoord - rectPos;

                // outside rectangle bounds -> discard immediately
                if (local.x < 0 || local.y < 0 ||
                    local.x > rectSize.x || local.y > rectSize.y)
                    discard;

                float sdf = roundedRectSDF(local, rectSize, radius);

                // sdf < 0 = inside
                // sdf > 0 = outside (discard)
                if (sdf > 0.0)
                    discard;

                // BORDER LOGIC:
                // inside border region if dist is between 0 and -strokeWeight
                if (-sdf < strokeWeight) {
                    fragColor = vec4(strokeColor.x/255, strokeColor.y/255, strokeColor.z/255, strokeColor.w/255);
                } else {
                    fragColor = vec4(fillColor.x/255, fillColor.y/255, fillColor.z/255, fillColor.w/255);
                }
            }
        )";

        GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);

        rectShader = glCreateProgram();
        glAttachShader(rectShader, vs);
        glAttachShader(rectShader, fs);
        glLinkProgram(rectShader);

        //glDeleteShader(vs);
        glDeleteShader(fs);

        fsSrc = R"(
            #version 420 core
            in vec2 pixelCoord;
            out vec4 fragColor;

            uniform vec2 center;     // center in pixels
            uniform vec2 radii;      // ellipse radii (w/2, h/2)

            uniform vec4 strokeColor;
            uniform vec4 fillColor;

            uniform float strokeWeight;

            // signed distance to ellipse: < 0 = inside
            float ellipseSDF(vec2 p, vec2 rad) {
                return (length(p / rad) - 1.0) * min(rad.x, rad.y);
            }

            void main() {
                // convert to local coordinates (center = 0,0)
                vec2 local = pixelCoord - center;

                float sdf = ellipseSDF(local, radii);

                // sdf < 0 = inside
                // sdf > 0 = outside (discard)
                if (sdf > 0.0)
                    discard;

                // BORDER LOGIC:
                // inside border region if dist is between 0 and -strokeWeight
                if (-sdf < strokeWeight) {
                    fragColor = vec4(strokeColor.x/255, strokeColor.y/255, strokeColor.z/255, strokeColor.w/255);
                } else {
                    fragColor = vec4(fillColor.x/255, fillColor.y/255, fillColor.z/255, fillColor.w/255);
                }
            }
        )";

        //vs = compileShader(GL_VERTEX_SHADER, vsSrc);
        fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);

        ellipseShader = glCreateProgram();
        glAttachShader(ellipseShader, vs);
        glAttachShader(ellipseShader, fs);
        glLinkProgram(ellipseShader);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    OpenGLRenderer::~OpenGLRenderer() {
    }

    void OpenGLRenderer::OnWindowResize(uint32_t width, uint32_t height) {
        windowWidth = width;
        windowHeight = height;
        glViewport(0, 0, width, height);
    }

    void OpenGLRenderer::rect(float x, float y, float w, float h, float r) {
        // pixel-space corners (top-left origin)
        float verts[] = {
            x,     y,
            x+w,   y,
            x+w,   y+h,
            x,     y+h
        };

        glBindVertexArray(rectVAO);

        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glUseProgram(rectShader);


        glUniform2f(glGetUniformLocation(rectShader, "screenSize"), (float)windowWidth, (float)windowHeight);
        glUniform2f(glGetUniformLocation(rectShader, "rectPos"),  x, y);
        glUniform2f(glGetUniformLocation(rectShader, "rectSize"), w, h);
        glUniform1f(glGetUniformLocation(rectShader, "radius"), r);
        glUniform4f(glGetUniformLocation(rectShader, "fillColor"), fillColor.r, fillColor.g, fillColor.b, fillColor.a);
        glUniform4f(glGetUniformLocation(rectShader, "strokeColor"), strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
        glUniform1f(glGetUniformLocation(rectShader, "strokeWeight"), m_strokeWeight);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    void OpenGLRenderer::ellipse(float x, float y, float w, float h) {
        // Rectangle covering the ellipse bounds
        float verts[] = {
            x,     y,
            x+w,   y,
            x+w,   y+h,
            x,     y+h
        };

        glBindVertexArray(ellipseVAO);

        glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glUseProgram(ellipseShader);

        glUniform2f(glGetUniformLocation(ellipseShader, "screenSize"), (float)windowWidth, (float)windowHeight);
        glUniform2f(glGetUniformLocation(ellipseShader, "center"), x + w * 0.5f, y + h * 0.5f);
        glUniform2f(glGetUniformLocation(ellipseShader, "radii"), w * 0.5f, h * 0.5f);
        glUniform4f(glGetUniformLocation(ellipseShader, "fillColor"), fillColor.r, fillColor.g, fillColor.b, fillColor.a);
        glUniform4f(glGetUniformLocation(ellipseShader, "strokeColor"), strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
        glUniform1f(glGetUniformLocation(ellipseShader, "strokeWeight"), m_strokeWeight);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    GLuint OpenGLRenderer::compileShader(GLenum type, const char *src) {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        return s;
    }
}
