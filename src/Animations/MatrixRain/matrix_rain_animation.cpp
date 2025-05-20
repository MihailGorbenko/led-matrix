#include "matrix_rain_animation.hpp"
#include <algorithm>

MatrixRainAnimation::MatrixRainAnimation()
    : Animation("matrix_rain", "Матрица"),
      speed(DEFAULT_MATRIX_RAIN_SPEED),
      speedSetting("speed", "Скорость", &speed, "matrix_rain", DEFAULT_MATRIX_RAIN_SPEED, 10, 200, 1),
      intensity(DEFAULT_MATRIX_RAIN_INTENSITY),
      intensitySetting("intensity", "Интенсивность", &intensity, "matrix_rain", DEFAULT_MATRIX_RAIN_INTENSITY, 0.1f, 1.0f, 0.01f),
      width(0), height(0)
{
    registerSetting(&speedSetting);
    registerSetting(&intensitySetting);
    loadConfig();
}

AnimationType MatrixRainAnimation::getType() const { return AnimationType::MatrixRain; }

void MatrixRainAnimation::render(LedMatrix& matrix, AudioAnalyzer*) {
    width = matrix.getWidth();
    height = matrix.getHeight();
    CRGB* leds = matrix.getLeds();
    fill_solid(leds, width * height, CRGB::Black);

    // Добавляем новые капли (интенсивность влияет на количество)
    int maxDrops = (int)(width * intensity);
    while ((int)drops.size() < maxDrops) {
        RainDrop drop;
        drop.x = random(0, width);
        drop.y = random(-height, 0);
        drop.length = random(5, height / 2 + 3);
        drop.layer = (random8() < 180) ? 0 : 1; // ~70% дальние, ~30% ближние
        drop.speed = (drop.layer == 0) ? random(2, 4) : random(1, 2); // дальние — медленнее
        drop.counter = 0;
        drops.push_back(drop);
    }

    // Обновляем и рисуем капли с учётом слоя (параллакс)
    for (auto& drop : drops) {
        drop.counter++;
        if (drop.counter >= drop.speed) {
            drop.y++;
            drop.counter = 0;
        }
        for (int i = 0; i < drop.length; ++i) {
            int py = drop.y - i;
            if (py >= 0 && py < height) {
                uint8_t brightness;
                if (drop.layer == 0) {
                    // дальний слой — тусклее и короче хвост
                    brightness = (i == 0) ? 120 : 60 - i * 6;
                } else {
                    // ближний слой — ярче и длиннее хвост
                    brightness = (i == 0) ? 255 : 120 - i * 8;
                }
                CRGB color = (i == 0)
                    ? CHSV(96, drop.layer == 0 ? 80 : 30, brightness)
                    : CHSV(96, 255, brightness);
                leds[matrix.XY(drop.x, py)] = color;
            }
        }
    }

    // Удаляем капли, которые вышли за низ
    drops.erase(
        std::remove_if(drops.begin(), drops.end(), [this](const RainDrop& d) {
            return d.y - d.length > height;
        }),
        drops.end()
    );

    matrix.update();
    delay(speed);
}