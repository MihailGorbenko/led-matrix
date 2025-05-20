#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <Arduino.h>

// Значения по умолчанию для LiveFire
#define DEFAULT_LIVE_FIRE_INTENSITY 0.7f
#define DEFAULT_LIVE_FIRE_SPEED 30  // мс между кадрами

class LiveFire : public Animation {
    float intensity;
    PersistantSetting<float> intensitySetting;

    int speed;
    PersistantSetting<int> speedSetting;

    uint8_t* heat;
    int width, height;

public:
    LiveFire();
    ~LiveFire();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};