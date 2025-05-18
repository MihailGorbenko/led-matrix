#include "starry_sky.hpp"
#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../../include/AnimationType.hpp"
#include <Arduino.h>
#include <vector>

// Статические переменные
std::vector<StarrySkyAnimation::Star> StarrySkyAnimation::stars;
std::vector<StarrySkyAnimation::ShootingStar> StarrySkyAnimation::shootingStars;
unsigned long StarrySkyAnimation::lastShootingStar = 0;
unsigned long StarrySkyAnimation::lastMeteorShower = 0;
bool StarrySkyAnimation::meteorShowerActive = false;
int StarrySkyAnimation::meteorShowerCount = 0;
int StarrySkyAnimation::meteorShowerOriginX = 0;
int StarrySkyAnimation::meteorShowerOriginY = 0;

StarrySkyAnimation::StarrySkyAnimation()
    : Animation("starrySky", "Звёздное небо"),
      starColorValue(0xFFFFFF),
      starColorSetting("starColor", "Цвет звёзд", &starColorValue, "starry_sky", 0xFFFFFF, 0, 0xFFFFFF, 1),
      cometColorValue(0x00FFFF),
      cometColorSetting("cometColor", "Цвет кометы", &cometColorValue, "starry_sky", 0x00FFFF, 0, 0xFFFFFF, 1),
      cometFrequency(10),
      cometFrequencySetting("cometFrequency", "Частота комет", &cometFrequency, "starry_sky", 10, 1, 60, 1),
      starsPercent(40),
      starsPercentSetting("starsPercent", "Процент звёзд", &starsPercent, "starry_sky", 40, 1, 100, 1)
{
    registerSetting(&starColorSetting);
    registerSetting(&cometColorSetting);
    registerSetting(&cometFrequencySetting);
    registerSetting(&starsPercentSetting);
    loadConfig();
}

AnimationType StarrySkyAnimation::getType() const { return AnimationType::StarrySky; }
bool StarrySkyAnimation::needsAudio() const { return false; }

void StarrySkyAnimation::render(LedMatrix& matrix, AudioAnalyzer* /*audio*/) {
    int width = matrix.getWidth();
    int height = matrix.getHeight();
    CRGB* leds = matrix.getLeds();

    // --- Параллакс: ближние и дальние звёзды ---
    int maxStars = (width * height) * starsPercent / 100;
    int farStars = maxStars * 2 / 3;
    int nearStars = maxStars - farStars;

    if (stars.size() != maxStars) {
        stars.clear();
        // Дальние звёзды (тусклые, медленные)
        for (int i = 0; i < farStars; ++i) {
            Star s;
            s.x = random(0, width);
            s.y = random(0, height);
            s.phase = random8();
            s.speed = random(1, 2); // медленное мерцание
            s.maxBright = random(30, 100); // тусклые
            s.layer = 0;
            stars.push_back(s);
        }
        // Ближние звёзды (ярче, быстрее)
        for (int i = 0; i < nearStars; ++i) {
            Star s;
            s.x = random(0, width);
            s.y = random(0, height);
            s.phase = random8();
            s.speed = random(2, 5); // быстрее
            s.maxBright = random(100, 255); // ярче
            s.layer = 1;
            stars.push_back(s);
        }
    }

    fill_solid(leds, width * height, CRGB::Black);

    // --- Плавное движение звёзд (вращение неба) ---
    static float skyShift = 0;
    skyShift += 0.01f; // скорость вращения

    // --- Мерцание и эволюция звёзд ---
    for (auto& s : stars) {
        s.phase += s.speed;
        float b = (sin8(s.phase) / 255.0f);
        uint8_t brightness = (uint8_t)(b * s.maxBright);

        // Случайное затухание и "рождение" новой звезды
        if (s.maxBright < 10 && random8() < 2) {
            s.x = random(0, width);
            s.y = random(0, height);
            s.phase = random8();
            s.speed = (s.layer == 0) ? random(1, 2) : random(2, 5);
            s.maxBright = (s.layer == 0) ? random(30, 100) : random(100, 255);
        } else if (random8() < 1) {
            // иногда звезда "тускнеет"
            if (s.maxBright > 10) s.maxBright--;
        }

        // Параллакс: дальние звёзды двигаются медленнее
        float shift = skyShift * (s.layer == 0 ? 0.3f : 1.0f);
        int sx = ((int)(s.x + shift)) % width;
        if (sx < 0) sx += width;

        CRGB color = CRGB(starColorValue);
        color.nscale8_video(brightness);
        leds[matrix.XY(sx, s.y)] = color;
    }

    // --- Кометы с хвостом ---
    unsigned long now = millis();
    if (now - lastCometTime > (1000 * cometFrequency)) {
        int side = random(0, 4);
        Comet comet;
        comet.life = width + height;
        switch (side) {
            case 0: comet.x = 0; comet.y = random(0, height); comet.dx = 1; comet.dy = random(-1, 2); break;
            case 1: comet.x = width - 1; comet.y = random(0, height); comet.dx = -1; comet.dy = random(-1, 2); break;
            case 2: comet.x = random(0, width); comet.y = 0; comet.dx = random(-1, 2); comet.dy = 1; break;
            case 3: comet.x = random(0, width); comet.y = height - 1; comet.dx = random(-1, 2); comet.dy = -1; break;
        }
        comets.push_back(comet);
        lastCometTime = now;
    }

    // --- Метеорный поток (серия комет из одной точки) ---
    if (!meteorShowerActive && now - lastMeteorShower > 20000 && random8() < 2) {
        meteorShowerActive = true;
        meteorShowerCount = random(3, 7);
        meteorShowerOriginX = random(0, width);
        meteorShowerOriginY = random(0, height);
        lastMeteorShower = now;
    }
    if (meteorShowerActive && meteorShowerCount > 0 && now - lastCometTime > 400) {
        Comet comet;
        comet.life = width + height;
        comet.x = meteorShowerOriginX;
        comet.y = meteorShowerOriginY;
        int dir = random(0, 4);
        switch (dir) {
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

    // --- Падающие звёзды (shooting stars) ---
    if (now - lastShootingStar > 15000 && random8() < 2) {
        ShootingStar s;
        s.x = random(0, width);
        s.y = 0;
        s.dx = 1.5f;
        s.dy = 1.0f;
        s.brightness = 255;
        s.life = width + height;
        shootingStars.push_back(s);
        lastShootingStar = now;
    }

    // --- Рисуем кометы с хвостом ---
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

    // --- Рисуем падающие звёзды ---
    for (auto it = shootingStars.begin(); it != shootingStars.end();) {
        for (int t = 0; t < 10; ++t) {
            int tx = (int)(it->x - it->dx * t);
            int ty = (int)(it->y - it->dy * t);
            if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
                uint8_t fade = 255 - t * 25;
                CRGB color = CRGB::White;
                color.nscale8_video(fade);
                leds[matrix.XY(tx, ty)] += color;
            }
        }
        it->x += it->dx;
        it->y += it->dy;
        it->life--;
        if (it->life <= 0 || it->x < 0 || it->x >= width || it->y < 0 || it->y >= height) {
            it = shootingStars.erase(it);
        } else {
            ++it;
        }
    }

    matrix.update();
}