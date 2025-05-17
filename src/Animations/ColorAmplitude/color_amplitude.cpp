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
    }

    AnimationType getType() const override { return AnimationType::ColorAmplitude; }
    bool needsAudio() const override { return true; }

    void render(LedMatrix& matrix, AudioAnalyzer* audio) override {
        if (!audio) return;
        audio->processAudio();
        uint16_t heights[MATRIX_WIDTH];
        audio->getNormalizedHeights(heights, MATRIX_WIDTH);

        CRGB* leds = matrix.getLeds();
        fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

        CRGB color = CRGB(colorValue);

        for (int x = 0; x < MATRIX_WIDTH; x++) {
            for (int y = MATRIX_HEIGHT - heights[x]; y < MATRIX_HEIGHT; y++) {
                if (colorValue == 0) {
                    // Динамический цвет: по высоте столбца
                    uint8_t hue = map(heights[x], 0, MATRIX_HEIGHT, 0, 255);
                    leds[matrix.XY(x, y)] = CHSV(hue, 255, 255);
                } else {
                    leds[matrix.XY(x, y)] = color;
                }
            }
        }
        matrix.update();
    }
};