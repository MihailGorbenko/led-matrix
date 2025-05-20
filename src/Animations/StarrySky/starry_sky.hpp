#pragma once

#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include <FastLED.h>
#include <vector>

// Значения по умолчанию для StarrySkyAnimation
#define DEFAULT_STARRY_SKY_STAR_COLOR      0xFFFFFF
#define DEFAULT_STARRY_SKY_COMET_COLOR     0x00FFFF
#define DEFAULT_STARRY_SKY_COMET_FREQUENCY 10
#define DEFAULT_STARRY_SKY_STARS_PERCENT   40

class StarrySkyAnimation : public Animation {
public:
    struct Star {
        int x, y;
        uint8_t phase;
        uint8_t speed;
        uint8_t maxBright;
        uint8_t layer;
    };
    struct Comet {
        int x, y;
        int dx, dy;
        int life;
    };
    struct ShootingStar {
        float x, y;
        float dx, dy;
        uint8_t brightness;
        int life;
    };

    static std::vector<Star> stars;
    static std::vector<ShootingStar> shootingStars;
    static std::vector<Comet> comets;
    static unsigned long lastShootingStar;
    static unsigned long lastMeteorShower;
    static unsigned long lastCometTime;
    static bool meteorShowerActive;
    static int meteorShowerCount;
    static int meteorShowerOriginX;
    static int meteorShowerOriginY;

    StarrySkyAnimation();

    AnimationType getType() const override;
    bool needsAudio() const override;
    void render(LedMatrix& matrix, AudioAnalyzer* audio) override;

private:
    int starColorValue;
    PersistantSetting<int> starColorSetting;
    int cometColorValue;
    PersistantSetting<int> cometColorSetting;
    int cometFrequency;
    PersistantSetting<int> cometFrequencySetting;
    int starsPercent;
    PersistantSetting<int> starsPercentSetting;
    bool cometsEnabled;
    PersistantSetting<bool> cometsEnabledSetting;
};