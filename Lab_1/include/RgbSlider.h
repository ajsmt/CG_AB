#pragma once

class RgbSlider
{
public:
    RgbSlider() : R(120), G(120), B(120) {};

    bool Render();

    int R;
    int G;
    int B;
};