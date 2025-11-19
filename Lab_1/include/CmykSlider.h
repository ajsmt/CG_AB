#pragma once

class CmykSlider
{
public:
    CmykSlider() : C(0), M(0), Y(0), K(0) {};

    bool Render();

    float C;
    float M;
    float Y;
    float K;
};