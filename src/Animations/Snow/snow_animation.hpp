#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>
#include <vector>

// Значения по умолчанию для SnowAnimation
#define DEFAULT_SNOW_DENSITY 0.08f   // Доля заполнения (0.0 - 1.0)
#define DEFAULT_SNOW_SPEED   0.5f    // Средняя скорость (пикселей за кадр)
#define DEFAULT_SNOW_WIND    0.0f    // Ветер (отрицательно - влево, положительно - вправо)

struct Snowflake {
    float x, y;
    float speed;
    float wind;
    uint8_t brightness;
    uint8_t size;
    bool melting = false;
    uint8_t meltStep = 0;
};

class SnowAnimation : public Animation {
    std::vector<Snowflake> flakes;
    float density;
    PersistantSetting<float> densitySetting;
    float speed;
    PersistantSetting<float> speedSetting;
    float wind;
    PersistantSetting<float> windSetting;
    int width, height;

public:
    SnowAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};

