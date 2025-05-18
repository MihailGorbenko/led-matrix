#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <Arduino.h>
#include <vector>

class StarrySkyAnimation : public Animation {
    int starColorValue;
    PersistantSetting<int> starColorSetting;
    int cometColorValue;
    PersistantSetting<int> cometColorSetting;
    int cometFrequency;
    PersistantSetting<int> cometFrequencySetting;
    int starsPercent;
    PersistantSetting<int> starsPercentSetting;

    struct Comet {
        int x, y, dx, dy, life;
    };
    std::vector<Comet> comets;
    unsigned long lastCometTime = 0;

    struct Star {
        int x, y;
        uint8_t phase;      // фаза для синусоидального мерцания
        uint8_t speed;      // скорость мерцания
        uint8_t maxBright;  // максимальная яркость
        uint8_t layer;      // 0 — дальние, 1 — ближние
    };
    static std::vector<Star> stars;

    struct ShootingStar {
        float x, y, dx, dy;
        uint8_t brightness;
        int life;
    };
    static std::vector<ShootingStar> shootingStars;
    static unsigned long lastShootingStar;
    static unsigned long lastMeteorShower;
    static bool meteorShowerActive;
    static int meteorShowerCount;
    static int meteorShowerOriginX, meteorShowerOriginY;

public:
    StarrySkyAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;
};