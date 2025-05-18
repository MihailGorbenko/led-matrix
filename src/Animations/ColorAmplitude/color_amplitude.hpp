#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include <Arduino.h>
#include <cmath>

class ColorAmplitudeAnimation : public Animation {
    int colorValue;
    PersistantSetting<int> colorSetting;

public:
    ColorAmplitudeAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};