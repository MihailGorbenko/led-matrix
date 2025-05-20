#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>
#include <vector>

#define DEFAULT_MATRIX_RAIN_SPEED 60 // мс между кадрами
#define DEFAULT_MATRIX_RAIN_INTENSITY 0.5f // 0.1 ... 1.0

struct RainDrop {
    int x;
    int y;
    int length;
    int speed;
    int counter;
    int layer; // 0 — дальний (медленный, тусклый), 1 — ближний (быстрый, яркий)
};

class MatrixRainAnimation : public Animation {
    std::vector<RainDrop> drops;
    int width, height;
    int speed;
    PersistantSetting<int> speedSetting;
    float intensity;
    PersistantSetting<float> intensitySetting;

public:
    MatrixRainAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override { return false; }
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};