#include "starry_sky.hpp"
#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <Arduino.h>
#include <vector>

// Статические переменные
std::vector<StarrySkyAnimation::Star> StarrySkyAnimation::stars;
std::vector<StarrySkyAnimation::ShootingStar> StarrySkyAnimation::shootingStars;
std::vector<StarrySkyAnimation::Comet> StarrySkyAnimation::comets;
unsigned long StarrySkyAnimation::lastShootingStar = 0;
unsigned long StarrySkyAnimation::lastMeteorShower = 0;
unsigned long StarrySkyAnimation::lastCometTime = 0;
bool StarrySkyAnimation::meteorShowerActive = false;
int StarrySkyAnimation::meteorShowerCount = 0;
int StarrySkyAnimation::meteorShowerOriginX = 0;
int StarrySkyAnimation::meteorShowerOriginY = 0;

StarrySkyAnimation::StarrySkyAnimation()
    : Animation("starrySky", "Звёздное небо"),
      starColorValue(DEFAULT_STARRY_SKY_STAR_COLOR),
      starColorSetting("starColor", "Цвет звёзд", &starColorValue, "starry_sky", DEFAULT_STARRY_SKY_STAR_COLOR, 0, 0xFFFFFF, 1),
      cometColorValue(DEFAULT_STARRY_SKY_COMET_COLOR),
      cometColorSetting("cometColor", "Цвет кометы", &cometColorValue, "starry_sky", DEFAULT_STARRY_SKY_COMET_COLOR, 0, 0xFFFFFF, 1),
      cometFrequency(DEFAULT_STARRY_SKY_COMET_FREQUENCY),
      cometFrequencySetting("cometFrequency", "Частота комет", &cometFrequency, "starry_sky", DEFAULT_STARRY_SKY_COMET_FREQUENCY, 1, 60, 1),
      starsPercent(DEFAULT_STARRY_SKY_STARS_PERCENT),
      starsPercentSetting("starsPercent", "Процент звёзд", &starsPercent, "starry_sky", DEFAULT_STARRY_SKY_STARS_PERCENT, 1, 100, 1),
      cometsEnabled(true),
      cometsEnabledSetting("cometsEnabled", "Показывать кометы", &cometsEnabled, "starry_sky", true)
{
    registerSetting(&starColorSetting);
    registerSetting(&cometColorSetting);
    registerSetting(&cometFrequencySetting);
    registerSetting(&starsPercentSetting);
    registerSetting(&cometsEnabledSetting);
    loadConfig();
}

AnimationType StarrySkyAnimation::getType() const { return AnimationType::StarrySky; }
bool StarrySkyAnimation::needsAudio() const { return false; }

void StarrySkyAnimation::render(LedMatrix& matrix, AudioAnalyzer* /*audio*/) {
    int width = matrix.getWidth();
    int height = matrix.getHeight();
    CRGB* leds = matrix.getLeds();
    fill_solid(leds, width * height, CRGB::Black);

    int maxStars = (width * height) * starsPercent / 100;
    int farStars = maxStars * 2 / 3;

    // Инициализация звёзд
    while (stars.size() < maxStars) {
        Star s;
        s.x = random(width);
        s.y = random(height);
        s.phase = random8();
        s.layer = (stars.size() < farStars) ? 0 : 1;
        s.speed = (s.layer == 0) ? random(1, 2) : random(2, 5);
        s.maxBright = (s.layer == 0) ? random(80, 150) : random(150, 255);
        stars.push_back(s);
    }

    static float skyShift = 0;
    skyShift += 0.01f;

    // Отображение звёзд с параллаксом и плавным колебанием яркости
    for (Star& s : stars) {
        s.phase += s.speed;
        float brightnessFactor = sin8(s.phase) / 255.0f;
        uint8_t brightness = brightnessFactor * s.maxBright;

        float shift = skyShift * (s.layer == 0 ? 0.3f : 1.0f);
        int sx = ((int)(s.x + shift)) % width;
        if (sx < 0) sx += width;

        CRGB color = CRGB(starColorValue);
        color.nscale8_video(brightness);
        leds[matrix.XY(sx, s.y)] = color;

        // Динамика яркости: случайные колебания maxBright
        if (random8() < 2) {
            int8_t delta = (random8() < 128) ? -1 : 1;
            s.maxBright = constrain(s.maxBright + delta, (s.layer == 0 ? 80 : 150), 255);
        }

        // Иногда полностью "перерождаем" звезду — вспышка/исчезновение
        if (random16() < 10) {
            s.x = random(width);
            s.y = random(height);
            s.phase = random8();
            s.layer = (stars.size() < farStars) ? 0 : 1;
            s.speed = (s.layer == 0) ? random(1, 2) : random(2, 5);
            s.maxBright = (s.layer == 0) ? random(80, 150) : random(150, 255);
        }
    }

    // Кометы
    if (cometsEnabled) {
        unsigned long now = millis();

        if (now - lastCometTime > 1000 * cometFrequency) {
            spawnComet(width, height);
            lastCometTime = now;
        }

        if (!meteorShowerActive && now - lastMeteorShower > 20000 && random8() < 2) {
            meteorShowerActive = true;
            meteorShowerCount = random(3, 7);
            meteorShowerOriginX = random(width);
            meteorShowerOriginY = random(height);
            lastMeteorShower = now;
        }

        if (meteorShowerActive && meteorShowerCount > 0 && now - lastCometTime > 400) {
            spawnMeteorComet(width, height);
            lastCometTime = now;
            meteorShowerCount--;
            if (meteorShowerCount == 0) meteorShowerActive = false;
        }

        renderComets(matrix);
    }

    matrix.update();
}
