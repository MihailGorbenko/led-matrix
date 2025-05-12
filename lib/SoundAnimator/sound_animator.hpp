#ifndef SOUND_ANIMATOR_HPP
#define SOUND_ANIMATOR_HPP

#include "led_matrix.hpp"
#include "audio_analyzer.hpp"
#include "matrix_task.hpp"
#include <functional> // Для std::function

class SoundAnimator : public MatrixTask {
public:
    SoundAnimator(LedMatrix& matrix);

    // Методы установки анимаций
    void setColorAmplitudeAnimation();
    void setGreenAmplitudeAnimation();
    void setPulsingRectangleAnimation(CRGB color = CRGB::Green);
    void setStarrySkyAnimation(CRGB color = CRGB::White);
    void setWaveAnimation(CRGB color = CRGB::Aqua);

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
    std::function<void()> currentRenderMethod = nullptr;

    // Методы отрисовки конкретных анимаций
    void renderColorAmplitude();
    void renderGreenAmplitude();
    void renderPulsingRectangle();
    void renderPulsingRectangle(CRGB color);
    void renderStarrySky();
    void renderStarrySky(CRGB color = CRGB::White);
    void renderWave(CRGB color);

    // FreeRTOS задача
    static void animationTask(void* param);
    TaskHandle_t animationTaskHandle = nullptr;
};

#endif // SOUND_ANIMATOR_HPP
