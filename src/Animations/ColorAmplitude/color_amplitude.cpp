#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include "color_amplitude.hpp"
#include <Arduino.h>
#include <cmath>

ColorAmplitudeAnimation::ColorAmplitudeAnimation()
    : Animation("colorAmplitude", "Цветная амплитуда"),
      colorValue(DEFAULT_COLOR_AMPLITUDE_COLOR),
      colorSetting("color", "Цвет", &colorValue, "color_amp", DEFAULT_COLOR_AMPLITUDE_COLOR, 0, 0xFFFFFF, 1)
{
    registerSetting(&colorSetting);
    loadConfig(); // Загружаем настройки из NVS при инициализации
}

AnimationType ColorAmplitudeAnimation::getType() const { return AnimationType::ColorAmplitude; }
bool ColorAmplitudeAnimation::needsAudio() const { return true; }

void ColorAmplitudeAnimation::render(LedMatrix& matrix, AudioAnalyzer* audio) {
    if (!audio) {
        Serial.println("[ColorAmplitude] audio is nullptr!");
        return;
    }
    audio->processAudio();

    int width = matrix.getWidth();
    int height = matrix.getHeight();

    uint16_t heights[MATRIX_WIDTH] = {0};
    audio->getNormalizedHeights(heights, width);

    CRGB* leds = matrix.getLeds();
    if (!leds) {
        Serial.println("[ColorAmplitude] leds is nullptr!");
        return;
    }
    fill_solid(leds, width * height, CRGB::Black);

    CRGB color = CRGB(colorValue);

    for (int x = 0; x < width; x++) {
        for (int y = height - heights[x]; y < height; y++) {
            int idx = matrix.XY(x, y);
            if (colorValue == 0) {
                uint8_t hue = map(heights[x], 0, height, 0, 255);
                leds[idx] = CHSV(hue, 255, 255);
            } else {
                leds[idx] = color;
            }
        }
    }
    matrix.update();
}