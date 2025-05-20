#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>

// Значения по умолчанию для WaveAnimation
#define DEFAULT_WAVE_SENSITIVITY      1.0f
#define DEFAULT_WAVE_FREQUENCY        0.3f
#define DEFAULT_WAVE_PHASE_INCREMENT  0.1f
#define DEFAULT_WAVE_COLOR            0x000000  // Чёрный (динамический градиент)

class WaveAnimation : public Animation {
private:
    float sensitivity;
    float frequency;
    float phaseIncrement;
    int colorValue;
    float currentPhase;

    PersistantSetting<float> sensitivitySetting;
    PersistantSetting<float> frequencySetting;
    PersistantSetting<float> phaseIncrementSetting;
    PersistantSetting<int> colorSetting;

public:
    WaveAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};