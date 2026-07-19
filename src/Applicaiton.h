#pragma once
#include <memory>

class Applicaiton
{
public: /* ---- Global ---- */
    explicit Applicaiton() = default;
    ~Applicaiton() = default;

    Applicaiton(const Applicaiton&) = delete;
    Applicaiton& operator=(const Applicaiton&) = delete;
    Applicaiton(Applicaiton&&) = default;
    Applicaiton& operator=(Applicaiton&&) = default;

    int RunApp();

private: /* ---- Local ---- */


};