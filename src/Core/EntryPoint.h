#pragma once

#include "Application.h"

extern Nova::Application* CreateApplication();

int main(int argc, const char* argv[]) {
    auto app = CreateApplication();

    app->Initialize();

    app->Run();

    delete app;
}