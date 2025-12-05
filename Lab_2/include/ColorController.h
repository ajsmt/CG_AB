#pragma once

#include "imgui.h"
#include <GL/gl.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "stb_image.h"

class ImageProcessor
{
public:
    struct Image
    {
        std::vector<unsigned char> data;
        int width = 0;
        int height = 0;
        int channels = 0;
    };

    static bool LoadImageFromFile(const char *filename, Image &outImg)
    {
        unsigned char *imgData = stbi_load(filename, &outImg.width, &outImg.height, &outImg.channels, 4);
        if (!imgData)
            return false;

        outImg.data.assign(imgData, imgData + (outImg.width * outImg.height * 4));
        outImg.channels = 4;
        stbi_image_free(imgData);
        return true;
    }

    static unsigned char GetLum(const Image &img, int x, int y)
    {
        x = std::max(0, std::min(x, img.width - 1));
        y = std::max(0, std::min(y, img.height - 1));

        int idx = (y * img.width + x) * 4;
        unsigned char r = img.data[idx];
        unsigned char g = img.data[idx + 1];
        unsigned char b = img.data[idx + 2];

        return static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);
    }

    static void ApplyMedian(const Image &src, Image &dst, int kernelSize = 3)
    {
        dst = src;
        int radius = kernelSize / 2;
        std::vector<unsigned char> window;
        window.reserve(kernelSize * kernelSize);

        for (int y = 0; y < src.height; ++y)
        {
            for (int x = 0; x < src.width; ++x)
            {
                window.clear();
                for (int ky = -radius; ky <= radius; ++ky)
                {
                    for (int kx = -radius; kx <= radius; ++kx)
                    {
                        window.push_back(GetLum(src, x + kx, y + ky));
                    }
                }
                std::sort(window.begin(), window.end());
                unsigned char med = window[window.size() / 2];

                int idx = (y * src.width + x) * 4;
                dst.data[idx] = med;
                dst.data[idx + 1] = med;
                dst.data[idx + 2] = med;
                dst.data[idx + 3] = 255;
            }
        }
    }

    static void ApplyBernsen(const Image &src, Image &dst, int kernelSize = 15, int contrastLimit = 15)
    {
        dst = src;
        int radius = kernelSize / 2;

        for (int y = 0; y < src.height; ++y)
        {
            for (int x = 0; x < src.width; ++x)
            {
                unsigned char minVal = 255;
                unsigned char maxVal = 0;

                for (int ky = -radius; ky <= radius; ++ky)
                {
                    for (int kx = -radius; kx <= radius; ++kx)
                    {
                        unsigned char val = GetLum(src, x + kx, y + ky);
                        if (val < minVal)
                            minVal = val;
                        if (val > maxVal)
                            maxVal = val;
                    }
                }

                int mid = (minVal + maxVal) / 2;
                int contrast = maxVal - minVal;
                unsigned char pixel = GetLum(src, x, y);
                unsigned char res = 0;

                if (contrast < contrastLimit)
                {
                    res = (mid >= 128) ? 255 : 0;
                }
                else
                {
                    res = (pixel >= mid) ? 255 : 0;
                }

                int idx = (y * src.width + x) * 4;
                dst.data[idx] = res;
                dst.data[idx + 1] = res;
                dst.data[idx + 2] = res;
                dst.data[idx + 3] = 255;
            }
        }
    }

    static void ApplyNiblack(const Image &src, Image &dst, int kernelSize = 15, float k = -0.2f)
    {
        dst = src;
        int radius = kernelSize / 2;
        int N = kernelSize * kernelSize;

        for (int y = 0; y < src.height; ++y)
        {
            for (int x = 0; x < src.width; ++x)
            {
                float sum = 0.0f;
                float sumSq = 0.0f;

                for (int ky = -radius; ky <= radius; ++ky)
                {
                    for (int kx = -radius; kx <= radius; ++kx)
                    {
                        unsigned char val = GetLum(src, x + kx, y + ky);
                        sum += val;
                        sumSq += (val * val);
                    }
                }

                float mean = sum / N;
                float variance = (sumSq / N) - (mean * mean);
                float sigma = std::sqrt(std::max(0.0f, variance));

                float threshold = mean + k * sigma;

                unsigned char pixel = GetLum(src, x, y);
                unsigned char res = (pixel > threshold) ? 255 : 0;

                int idx = (y * src.width + x) * 4;
                dst.data[idx] = res;
                dst.data[idx + 1] = res;
                dst.data[idx + 2] = res;
                dst.data[idx + 3] = 255;
            }
        }
    }
};

class ColorController
{
private:
    GLuint originalTex = 0;
    GLuint medianTex = 0;
    GLuint bernsenTex = 0;
    GLuint niblackTex = 0;

    ImageProcessor::Image srcImg;

    GLuint CreateGLTexture(const ImageProcessor::Image &img)
    {
        if (img.data.empty())
            return 0;
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data.data());
        return texID;
    }

public:
    ColorController() {}

    ~ColorController()
    {
        ClearResults();
        if (originalTex)
            glDeleteTextures(1, &originalTex);
    }

    void LoadImage(const char *filepath)
    {
        if (ImageProcessor::LoadImageFromFile(filepath, srcImg))
        {
            if (originalTex)
                glDeleteTextures(1, &originalTex);
            originalTex = CreateGLTexture(srcImg);

            ClearResults();
        }
        else
        {
            std::cerr << "Error loading image: " << filepath << std::endl;
        }
    }

    void ClearResults()
    {
        if (medianTex)
        {
            glDeleteTextures(1, &medianTex);
            medianTex = 0;
        }
        if (bernsenTex)
        {
            glDeleteTextures(1, &bernsenTex);
            bernsenTex = 0;
        }
        if (niblackTex)
        {
            glDeleteTextures(1, &niblackTex);
            niblackTex = 0;
        }
    }

    void OnBtnMedian()
    {
        if (srcImg.data.empty())
            return;
        ImageProcessor::Image res;
        ImageProcessor::ApplyMedian(srcImg, res, 3);
        if (medianTex)
            glDeleteTextures(1, &medianTex);
        medianTex = CreateGLTexture(res);
    }

    void OnBtnBernsen()
    {
        if (srcImg.data.empty())
            return;
        ImageProcessor::Image res;
        ImageProcessor::ApplyBernsen(srcImg, res, 15, 15);
        if (bernsenTex)
            glDeleteTextures(1, &bernsenTex);
        bernsenTex = CreateGLTexture(res);
    }

    void OnBtnNiblack()
    {
        if (srcImg.data.empty())
            return;
        ImageProcessor::Image res;
        ImageProcessor::ApplyNiblack(srcImg, res, 15, -0.2f);
        if (niblackTex)
            glDeleteTextures(1, &niblackTex);
        niblackTex = CreateGLTexture(res);
    }

    void Render()
    {
        ImGui::Begin("Управление");

        ImGui::Text("V_15");
        ImGui::Separator();

        static char fileNameBuf[128] = "noise.jpg";
        ImGui::InputText("File Path", fileNameBuf, 128);

        if (ImGui::Button("Загрузить"))
        {
            LoadImage(fileNameBuf);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Методы:");

        if (ImGui::Button("Медианный фильтр (3x3)"))
        {
            OnBtnMedian();
        }

        if (ImGui::Button("Бернсен"))
        {
            OnBtnBernsen();
        }

        if (ImGui::Button("Ниблак"))
        {
            OnBtnNiblack();
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        if (ImGui::Button("Очистка"))
        {
            ClearResults();
        }
        ImGui::PopStyleColor();

        ImGui::End();

        ImGui::Begin("Исходное");
        if (originalTex)
        {
            float w = ImGui::GetWindowWidth();
            float h = w * ((float)srcImg.height / (float)srcImg.width);
            ImGui::Image((void *)(intptr_t)originalTex, ImVec2(w - 20, h));
        }
        else
        {
        }
        ImGui::End();

        if (medianTex)
        {
            ImGui::Begin("Медианный фильтр");
            float w = ImGui::GetWindowWidth();
            float h = w * ((float)srcImg.height / (float)srcImg.width);
            ImGui::Image((void *)(intptr_t)medianTex, ImVec2(w - 20, h));
            ImGui::End();
        }

        if (bernsenTex)
        {
            ImGui::Begin("Бернсен");
            float w = ImGui::GetWindowWidth();
            float h = w * ((float)srcImg.height / (float)srcImg.width);
            ImGui::Image((void *)(intptr_t)bernsenTex, ImVec2(w - 20, h));
            ImGui::End();
        }

        if (niblackTex)
        {
            ImGui::Begin("Ниблак");
            float w = ImGui::GetWindowWidth();
            float h = w * ((float)srcImg.height / (float)srcImg.width);
            ImGui::Image((void *)(intptr_t)niblackTex, ImVec2(w - 20, h));
            ImGui::End();
        }
    }
};