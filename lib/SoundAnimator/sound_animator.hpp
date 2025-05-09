#ifndef SOUND_ANIMATOR_HPP
#define SOUND_ANIMATOR_HPP

#include "led_matrix.hpp"
#include "audio_analyzer.hpp"
#include "matrix_task.hpp"

enum AnimationType {
    NONE,
    COLOR_AMPLITUDE,
    GREEN_AMPLITUDE
};

class SoundAnimator : public MatrixTask {
public:
    SoundAnimator(LedMatrix& matrix, AudioAnalyzer& analyzer);

    // Методы установки анимаций
    void setColorAmplitudeAnimation();
    void setGreenAmplitudeAnimation();
    
    // Обновление текущей анимации
    void update();

    // Управление задачей
    void startTask() override;
    void stopTask() override;

    // Доступ к анализатору
    AudioAnalyzer& getAnalyzer() { return audioAnalyzer; }

private:
    LedMatrix& ledMatrix;
    AudioAnalyzer& audioAnalyzer;

    unsigned long lastUpdateTime = 0;
    bool isAnimating = false;
    AnimationType currentAnimation = NONE;

    // Методы отрисовки конкретных анимаций
    void renderColorAmplitude();
    void renderGreenAmplitude();

    // FreeRTOS задача
    static void animationTask(void* param);
    TaskHandle_t animationTaskHandle = nullptr;
};

#endif // SOUND_ANIMATOR_HPP
