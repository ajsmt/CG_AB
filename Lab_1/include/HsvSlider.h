#pragma once

class HsvSlider
{
public:
    HsvSlider() : H(0), S(0), V(0) {};

    bool Render();

    int H;
    float S;
    float V;
};