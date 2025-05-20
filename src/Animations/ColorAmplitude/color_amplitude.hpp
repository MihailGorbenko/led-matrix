#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>

// Значения по умолчанию для ColorAmplitudeAnimation
#define DEFAULT_COLOR_AMPLITUDE_COLOR 0x000000  // 0 = динамический цвет

class ColorAmplitudeAnimation : public Animation {
private:
    int colorValue;
    PersistantSetting<int> colorSetting;

public:
    ColorAmplitudeAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};