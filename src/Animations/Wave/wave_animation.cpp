#include "wave_animation.hpp"
#include <Arduino.h>
#include <cmath>

WaveAnimation::WaveAnimation()
    : Animation("wave", "Волна"),
      sensitivity(DEFAULT_WAVE_SENSITIVITY),
      frequency(DEFAULT_WAVE_FREQUENCY),
      phaseIncrement(DEFAULT_WAVE_PHASE_INCREMENT),
      colorValue(DEFAULT_WAVE_COLOR),
      currentPhase(0.0f),
      sensitivitySetting("sensitivity", "Чувствительность", &sensitivity, "wave_sens", DEFAULT_WAVE_SENSITIVITY, 0.1f, 10.0f, 0.1f),
      frequencySetting("frequency", "Частота волны", &frequency, "wave_freq", DEFAULT_WAVE_FREQUENCY, 0.01f, 5.0f, 0.01f),
      phaseIncrementSetting("phaseIncrement", "Приращение фазы", &phaseIncrement, "wave_phase", DEFAULT_WAVE_PHASE_INCREMENT, 0.01f, 1.0f, 0.01f),
      colorSetting("color", "Цвет", &colorValue, "wave_color", DEFAULT_WAVE_COLOR, 0, 0xFFFFFF, 1)
{
    registerSetting(&sensitivitySetting);
    registerSetting(&frequencySetting);
    registerSetting(&phaseIncrementSetting);
    registerSetting(&colorSetting);
    loadConfig();
}

AnimationType WaveAnimation::getType() const {
    return AnimationType::Wave;
}

bool WaveAnimation::needsAudio() const {
    return true;
}

void WaveAnimation::render(LedMatrix& matrix, AudioAnalyzer* audio) {
    if (!audio) {
        Serial.println("[WaveAnimation] audio is nullptr!");
        return;
    }

    audio->processAudio();

    int width = matrix.getWidth();
    int height = matrix.getHeight();
    CRGB* leds = matrix.getLeds();
    if (!leds) {
        Serial.println("[WaveAnimation] leds is nullptr!");
        return;
    }

    // Получаем энергию сигнала и статистику
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

    // Вычисляем высоту волны
    uint8_t waveH = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, 1, height / 2);
    waveH = constrain(waveH, 1, height / 2);

    // Фаза волны
    static float phase = 0;
    phase += phaseIncrement;
    phase = fmod(phase, 2 * PI); // Ограничиваем phase

    // Очищаем матрицу
    fill_solid(leds, width * height, CRGB::Black);

    // Рисуем волну
    int cy = height / 2;
    for (int x = 0; x < width; x++) {
        int wy = cy + sinf(phase + x * frequency) * waveH;
        wy = constrain(wy, 0, height - 1);

        if (colorValue == 0) {
            uint8_t hue = map(wy, 0, height - 1, 0, 255);
            leds[matrix.XY(x, wy)] = CHSV(hue, 255, 255);
        } else {
            leds[matrix.XY(x, wy)] = CRGB(colorValue);
        }

        // Отражённая волна
        int my = cy - (wy - cy);
        my = constrain(my, 0, height - 1);
        if (colorValue == 0) {
            uint8_t hue = map(my, 0, height - 1, 0, 255);
            leds[matrix.XY(x, my)] = CHSV(hue, 255, 255);
        } else {
            leds[matrix.XY(x, my)] = CRGB(colorValue);
        }
    }

    matrix.update();
}