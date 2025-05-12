#ifndef SOUND_ANIMATOR_HPP
#define SOUND_ANIMATOR_HPP

#include "led_matrix.hpp"
#include "audio_analyzer.hpp"
#include "matrix_task.hpp"

class SoundAnimator : public MatrixTask {
public:
    SoundAnimator(LedMatrix& matrix);

    // Методы установки анимаций
    void setColorAmplitudeAnimation();
    void setGreenAmplitudeAnimation();
    void setPulsingRingAnimation();

    // Обновление текущей анимации
    void update();
    void initializeAudioAnalyzer();

    // Управление задачей
    void startTask() override;
    void stopTask() override;

    // Метод для получения ссылки на AudioAnalyzer
    AudioAnalyzer& getAudioAnalyzer();

private:
    LedMatrix& ledMatrix;
    AudioAnalyzer audioAnalyzer;

    unsigned long lastUpdateTime = 0;
    bool isAnimating = false;

    // Указатель на текущий метод рендера
    void (SoundAnimator::*currentRenderMethod)() = nullptr;

    // Методы отрисовки конкретных анимаций
    void renderColorAmplitude();
    void renderGreenAmplitude();
    void renderPulsingRing();

    // FreeRTOS задача
    static void animationTask(void* param);
    TaskHandle_t animationTaskHandle = nullptr;
};

#endif // SOUND_ANIMATOR_HPP
