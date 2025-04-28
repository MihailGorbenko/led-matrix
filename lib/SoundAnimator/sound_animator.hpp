#ifndef SOUND_ANIMATOR_H
#define SOUND_ANIMATOR_H

#include <functional>
#include "led_matrix.hpp"
#include "audio_analyzer.hpp"

enum AnimationType {
    COLOR_AMPLITUDE,
    GREEN_AMPLITUDE
};

class SoundAnimator {
private:
    LedMatrix& ledMatrix;
    AudioAnalyzer& audioAnalyzer;

    unsigned long lastUpdateTime; // Время последнего обновления
    bool isAnimating; // Флаг состояния анимации
    AnimationType currentAnimation; // Текущая анимация
    std::function<CRGB(uint8_t)> currentColorFunc; // Текущая функция цвета

    // Общий метод для визуализации амплитуды
    void renderAmplitude(std::function<CRGB(uint8_t)> colorFunc);

    // Задача для обновления анимации
    static void animationTask(void* param);

    // Указатель на задачу FreeRTOS
    TaskHandle_t animationTaskHandle = nullptr;

public:
    SoundAnimator(LedMatrix& matrix, AudioAnalyzer& analyzer);

    void startColorAmplitude();
    void startGreenAmplitude();
    void update();
    void startTask();
    void stopTask();
};

#endif // SOUND_ANIMATOR_H