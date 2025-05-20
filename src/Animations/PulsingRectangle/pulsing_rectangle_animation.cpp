#include "pulsing_rectangle_animation.hpp"
#include <Arduino.h>
#include <algorithm>



PulsingRectangle::PulsingRectangle()
    : Animation("pulsingRectangle", "Прямоугольник"),
      sensitivity(DEFAULT_PULSING_RECTANGLE_SENSITIVITY),
      colorValue(0),
      minSize(DEFAULT_RECTANGLE_MIN_SIZE),
      sensitivitySetting("sensitivity", "Чувствительность", &sensitivity, "pulse_sens", DEFAULT_PULSING_RECTANGLE_SENSITIVITY, 0.1f, 10.0f, 0.1f),
      colorSetting("color", "Цвет", &colorValue, "pulse_color", 0, 0, DEFAULT_PULSING_RECTANGLE_COLOR, 1),
      minSizeSetting("minSize", "Мин. размер", &minSize, "pulse_min_size", DEFAULT_RECTANGLE_MIN_SIZE, 1, 32, 1)
{
    registerSetting(&sensitivitySetting);
    registerSetting(&colorSetting);
    registerSetting(&minSizeSetting);
    loadConfig();
}

AnimationType PulsingRectangle::getType() const {
    return AnimationType::PulsingRectangle;
}

bool PulsingRectangle::needsAudio() const {
    return true;
}

void PulsingRectangle::render(LedMatrix& matrix, AudioAnalyzer* audio) {
    if (!audio) {
        Serial.println("[PulsingRectangle] audio is nullptr!");
        return;
    }

    audio->processAudio();
    float logRmsEnergy = audio->getTotalLogRmsEnergy();
    float minLogPower = audio->getMinLogPower();
    float maxLogPower = audio->getMaxLogPower();

    // Усиление сигнала с учётом чувствительности
    float amplified = logRmsEnergy * sensitivity;

    // Используем статистику для определения диапазона
    float dynamicMinLogPower = minLogPower;
    float dynamicMaxLogPower = maxLogPower;

    // Убедимся, что диапазон корректен
    dynamicMinLogPower = constrain(dynamicMinLogPower, 1.0f, 50.0f);
    dynamicMaxLogPower = constrain(dynamicMaxLogPower, dynamicMinLogPower + 1.0f, 100.0f);

    int width = matrix.getWidth();
    int height = matrix.getHeight();

    // Вычисляем размеры прямоугольника
    uint8_t w = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, minSize, width);
    uint8_t h = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, minSize, height);

    // Ограничиваем размеры
    w = constrain(w, minSize, width);
    h = constrain(h, minSize, height);

    // Получаем массив светодиодов
    CRGB* leds = matrix.getLeds();
    fill_solid(leds, width * height, CRGB::Black);

    // Вычисляем координаты прямоугольника
    int cx = width / 2, cy = height / 2;
    int sx = cx - w / 2, sy = cy - h / 2, ex = cx + w / 2 - 1, ey = cy + h / 2 - 1;

    // Ограничиваем координаты
    sx = constrain(sx, 0, width - 1);
    sy = constrain(sy, 0, height - 1);
    ex = constrain(ex, 0, width - 1);
    ey = constrain(ey, 0, height - 1);

    CRGB baseColor = (colorValue == 0) ? CRGB::White : CRGB(colorValue);

    // Рисуем прямоугольник (только границы)
    for (int x = sx; x <= ex; x++) {
        leds[matrix.XY(x, sy)] = baseColor;
        leds[matrix.XY(x, ey)] = baseColor;
    }
    for (int y = sy; y <= ey; y++) {
        leds[matrix.XY(sx, y)] = baseColor;
        leds[matrix.XY(ex, y)] = baseColor;
    }

    matrix.update();
}