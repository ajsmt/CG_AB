#include "CmykSlider.h"
#include "RgbSlider.h"
#include "HsvSlider.h"

#pragma once

class ColorController
{
public:
    ColorController()
    {
        RGBtoCMYK();
        RGBtoHSV();
    };

    RgbSlider RGB;
    CmykSlider CMYK;
    HsvSlider HSV;

    void Render();

    void RGBtoCMYK();
    void RGBtoHSV();
    void CMYKtoRGB();
    void HSVtoRGB();

    void ColorPanelRender();
    void ColorPaletteRender();
};