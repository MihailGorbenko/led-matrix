#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>

#define DEFAULT_PULSING_RECTANGLE_SENSITIVITY 0.9f
#define DEFAULT_RECTANGLE_MIN_SIZE 1
#define DEFAULT_PULSING_RECTANGLE_COLOR 0xFFFFFF  // Белый по умолчанию

class PulsingRectangle : public Animation {
private:
    float sensitivity;
    int colorValue;
    int minSize;

    PersistantSetting<float> sensitivitySetting;
    PersistantSetting<int> colorSetting;
    PersistantSetting<int> minSizeSetting;

public:
    PulsingRectangle();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};