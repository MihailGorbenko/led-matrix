#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>
#include <vector>

#define DEFAULT_LIGHTNING_INTERVAL 4000 // мс между молниями

struct LightningBolt {
    std::vector<std::pair<int, int>> path; // (x, y) координаты
    uint8_t age = 0;
    uint8_t maxAge = 6;
};

class LightningAnimation : public Animation {
    unsigned long lastLightning = 0;
    int interval; // Было: unsigned long interval;
    PersistantSetting<int> intervalSetting;
    std::vector<LightningBolt> bolts;
    int width, height;

public:
    LightningAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override { return false; }
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};