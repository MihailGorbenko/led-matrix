#include "snow_animation.hpp"
#include <FastLED.h>
#include <vector>
#include <algorithm>

SnowAnimation::SnowAnimation()
    : Animation("snow", "Снег"),
      density(DEFAULT_SNOW_DENSITY),
      densitySetting("density", "Плотность", &density, "snow", DEFAULT_SNOW_DENSITY, 0.01f, 0.3f, 0.01f),
      speed(DEFAULT_SNOW_SPEED),
      speedSetting("speed", "Скорость", &speed, "snow", DEFAULT_SNOW_SPEED, 0.1f, 2.0f, 0.01f),
      wind(DEFAULT_SNOW_WIND),
      windSetting("wind", "Ветер", &wind, "snow", DEFAULT_SNOW_WIND, -1.0f, 1.0f, 0.01f),
      width(0), height(0)
{
    registerSetting(&densitySetting);
    registerSetting(&speedSetting);
    registerSetting(&windSetting);
    loadConfig();
}

AnimationType SnowAnimation::getType() const { return AnimationType::Snow; }
bool SnowAnimation::needsAudio() const { return false; }

void SnowAnimation::render(LedMatrix& matrix, AudioAnalyzer*) {
    if (width != matrix.getWidth() || height != matrix.getHeight()) {
        width = matrix.getWidth();
        height = matrix.getHeight();
        flakes.clear();
    }

    CRGB* leds = matrix.getLeds();
    fill_solid(leds, width * height, CRGB::Black);

    // Добавляем новые снежинки до нужной плотности
    int targetCount = (int)(width * height * density);
    while ((int)flakes.size() < targetCount) {
        Snowflake f;
        f.x = random(0, width);
        f.y = random(-height / 2, 0); // появление чуть выше экрана
        f.size = random(1, 3); // 1 или 2
        f.speed = speed * (0.5f + f.size * 0.4f) * (0.7f + random(0, 60) / 100.0f); // немного разная скорость
        f.wind = wind + (random(-10, 11) / 100.0f); // индивидуальный ветерок
        f.brightness = random(180, 255);
        f.melting = false;
        f.meltStep = 0;
        flakes.push_back(f);
    }

    // Обновляем и рисуем снежинки
    for (auto& f : flakes) {
        f.y += f.speed;
        f.x += f.wind + sinf(f.y * 0.2f + f.x) * 0.3f; // покачивание

        // Ограничиваем по краям
        if (f.x < 0) f.x += width;
        if (f.x >= width) f.x -= width;

        // Включаем процесс таяния
        if (f.y >= height - 1 && !f.melting) {
            f.melting = true;
            f.meltStep = 0;
        }
        if (f.melting) {
            f.brightness = qsub8(f.brightness, 20); // постепенно тускнеет
            f.meltStep++;
            if (f.brightness == 0 || f.meltStep > 10) {
                f.y = height + 1; // пометить на удаление
            }
        }

        // Рисуем снежинку (размер 1 или 2)
        for (int dx = 0; dx < f.size; dx++) {
            int ix = (int)f.x + dx;
            if (ix >= 0 && ix < width) {
                int iy = (int)f.y;
                if (iy >= 0 && iy < height) {
                    uint8_t draw_brightness = qadd8(f.brightness, random(-5, 6));
                    leds[matrix.XY(ix, iy)] += CHSV(0, 0, draw_brightness);
                }
            }
        }
    }

    // Удаляем снежинки, которые вышли за низ или растаяли
    flakes.erase(
        std::remove_if(flakes.begin(), flakes.end(), [this](const Snowflake& f) {
            return f.y >= height;
        }),
        flakes.end()
    );

    matrix.update();
    delay(30);
}

