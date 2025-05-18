#include "../../Animation/animation.hpp"
#include "../../LedMatrix/led_matrix.hpp"
#include "../../AudioAnalyzer/audio_analyzer.hpp"
#include "../../../include/AnimationType.hpp"
#include <Arduino.h>
#include <cmath>

class ColorAmplitudeAnimation : public Animation {
    int colorValue;
    PersistantSetting<int> colorSetting;

public:
    ColorAmplitudeAnimation()
        : Animation("colorAmplitude", "Цветная амплитуда"),
          colorValue(0), // 0 = CRGB::Black, значит динамический цвет
          colorSetting("color", "Цвет", &colorValue, "color_amp", 0, 0, 0xFFFFFF, 1)
    {
        registerSetting(&colorSetting);
        loadConfig(); // Загружаем настройки из NVS при инициализации
    }

    AnimationType getType() const override { return AnimationType::ColorAmplitude; }
    bool needsAudio() const override { return true; }

    void render(LedMatrix& matrix, AudioAnalyzer* audio) override {
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
};