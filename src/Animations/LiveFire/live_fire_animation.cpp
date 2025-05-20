#include "live_fire_animation.hpp"
#include <FastLED.h>

LiveFire::LiveFire()
    : Animation("live_fire", "Огонь"),
      intensity(DEFAULT_LIVE_FIRE_INTENSITY),
      intensitySetting("fire_intensity", "Интенсивность", &intensity, "live_fire", DEFAULT_LIVE_FIRE_INTENSITY, 0.1f, 1.0f, 0.01f),
      speed(DEFAULT_LIVE_FIRE_SPEED),
      speedSetting("fire_speed", "Скорость", &speed, "live_fire", DEFAULT_LIVE_FIRE_SPEED, 5, 100, 1),
      heat(nullptr), width(0), height(0)
{
    registerSetting(&intensitySetting);
    registerSetting(&speedSetting);
    loadConfig();
}

LiveFire::~LiveFire() {
    delete[] heat;
}

AnimationType LiveFire::getType() const {
    return AnimationType::LiveFire;
}

bool LiveFire::needsAudio() const {
    return false;
}

void LiveFire::render(LedMatrix& matrix, AudioAnalyzer* /*audio*/) {
    if (width != matrix.getWidth() || height != matrix.getHeight()) {
        delete[] heat;
        width = matrix.getWidth();
        height = matrix.getHeight();
        heat = new uint8_t[width * height]{};
    }

    CRGB* leds = matrix.getLeds();
    if (!leds) return;

    float fireIntensity = intensity;
    int cooling = 65 - (int)(fireIntensity * 45); // Усиленное охлаждение
    int sparking = 40 + (int)(fireIntensity * 100); // Меньше искр

    // 1. Охлаждение (по строкам, как в Fire2012)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int idx = y * width + x;
            int cool = cooling;
            if (y < height / 3) cool += 40;
            heat[idx] = qsub8(heat[idx], random(0, (cool * 10) / height + 2));
        }
    }

    // 2. Перенос вверх (усреднение с нижними пикселями)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height - 2; y++) { // снизу вверх!
            int xoff = x + random(-1, 2);
            if (xoff < 0) xoff = 0;
            if (xoff >= width) xoff = width - 1;
            int idx = y * width + x;
            int above = (y + 1) * width + xoff;
            int above2 = (y + 2) * width + xoff;
            heat[idx] = (heat[above2] * 2 + heat[above] * 2 + heat[idx]) / 5;
        }
    }

    // 3. Искры внизу (разогрев нижних пикселей)
    for (int x = 0; x < width; x++) {
        if (random(255) < sparking) {
            int y = height - 1 - random(2); // y = height-1 или height-2 (низ)
            int idx = y * width + x;
            heat[idx] = qadd8(heat[idx], random(160, 255));
        }
    }

    // 4. Ограничение максимального heat
    for (int i = 0; i < width * height; i++) {
        if (heat[i] > 192) heat[i] = 192; // максимум для палитры огня
    }

    // 5. Отображение (HeatColor)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            uint8_t colorindex = scale8(heat[idx], 192); // максимум 192
            leds[matrix.XY(x, y)] = ColorFromPalette(HeatColors_p, colorindex);
            if (y < 3) { // затухание в самом верху
                leds[matrix.XY(x, y)].fadeToBlackBy(80);
            }
        }
    }

    matrix.update();
    delay(speed); // Используем настраиваемую задержку
}