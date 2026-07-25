#pragma once
#include <memory>

class Application
{
public: /* ---- Global ---- */
    explicit Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;

    int RunApp();

private: /* ---- Local ---- */


};