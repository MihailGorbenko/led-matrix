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

    int maxStars = (width * height) * starsPercent / 100;
    int farStars = maxStars * 2 / 3;
    int nearStars = maxStars - farStars;

    int currentStars = stars.size();
    int maxTotalStars = width * height;

    // Добавляем звезды, если нужно
    if (currentStars < maxTotalStars) {
        int toAdd = maxStars - currentStars;
        if (toAdd > 0) {
            for (int i = 0; i < toAdd; ++i) {
                Star s;
                s.x = random(0, width);
                s.y = random(0, height);
                s.phase = random8();
                if (i < toAdd * 2 / 3) {
                    s.speed = random(1, 2);
                    s.maxBright = random(30, 100);
                    s.layer = 0;
                } else {
                    s.speed = random(2, 5);
                    s.maxBright = random(100, 255);
                    s.layer = 1;
                }
                stars.push_back(s);
            }
        }
    }

    fill_solid(leds, width * height, CRGB::Black);

    static float skyShift = 0;
    skyShift += 0.01f;

    for (int i = 0; i < maxStars && i < stars.size(); ++i) {
        auto& s = stars[i];

        s.phase += s.speed;
        float b = (sin8(s.phase) / 255.0f);
        uint8_t brightness = (uint8_t)(b * s.maxBright);

        if (s.maxBright < 10 && random8() < 2) {
            s.x = random(0, width);
            s.y = random(0, height);
            s.phase = random8();
            s.speed = (s.layer == 0) ? random(1, 2) : random(2, 5);
            s.maxBright = (s.layer == 0) ? random(30, 100) : random(100, 255);
        } else if (random8() < 1) {
            if (s.maxBright > 10) s.maxBright--;
        }

        float shift = skyShift * (s.layer == 0 ? 0.3f : 1.0f);
        int sx = ((int)(s.x + shift)) % width;
        if (sx < 0) sx += width;

        CRGB color = CRGB(starColorValue);
        color.nscale8_video(brightness);
        leds[matrix.XY(sx, s.y)] = color;
    }

    if (cometsEnabled) {
        unsigned long now = millis();
        if (now - lastCometTime > (1000 * cometFrequency)) {
            Comet comet;
            comet.life = width + height;
            switch (random(4)) {
                case 0: comet.x = 0; comet.y = random(height); comet.dx = 1; comet.dy = random(-1, 2); break;
                case 1: comet.x = width - 1; comet.y = random(height); comet.dx = -1; comet.dy = random(-1, 2); break;
                case 2: comet.x = random(width); comet.y = 0; comet.dx = random(-1, 2); comet.dy = 1; break;
                case 3: comet.x = random(width); comet.y = height - 1; comet.dx = random(-1, 2); comet.dy = -1; break;
            }
            comets.push_back(comet);
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
            Comet comet;
            comet.life = width + height;
            comet.x = meteorShowerOriginX;
            comet.y = meteorShowerOriginY;
            switch (random(4)) {
                case 0: comet.dx = 1; comet.dy = 1; break;
                case 1: comet.dx = -1; comet.dy = 1; break;
                case 2: comet.dx = 1; comet.dy = -1; break;
                case 3: comet.dx = -1; comet.dy = -1; break;
            }
            comets.push_back(comet);
            lastCometTime = now;
            meteorShowerCount--;
            if (meteorShowerCount == 0) meteorShowerActive = false;
        }

        for (auto it = comets.begin(); it != comets.end();) {
            const int tailLength = 6;
            for (int t = 0; t < tailLength; ++t) {
                int tailX = it->x - it->dx * t;
                int tailY = it->y - it->dy * t;
                if (tailX >= 0 && tailX < width && tailY >= 0 && tailY < height) {
                    uint8_t tailFade = 255 - (t * (200 / tailLength));
                    CRGB tailColor = CRGB(cometColorValue);
                    tailColor.nscale8_video(tailFade);
                    leds[matrix.XY(tailX, tailY)] += tailColor;
                }
            }

            if (it->x >= 0 && it->x < width && it->y >= 0 && it->y < height) {
                leds[matrix.XY(it->x, it->y)] = CRGB(cometColorValue);
            }

            it->x += it->dx;
            it->y += it->dy;
            it->life--;
            if (it->life <= 0 || it->x < 0 || it->x >= width || it->y < 0 || it->y >= height) {
                it = comets.erase(it);
            } else {
                ++it;
            }
        }
    }

    matrix.update();
}
