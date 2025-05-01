#ifndef SOUND_ANIMATOR_HPP
#define SOUND_ANIMATOR_HPP

#include <functional>
#include "led_matrix.hpp"
#include "audio_analyzer.hpp"
#include "matrix_task.hpp"


enum AnimationType {
    COLOR_AMPLITUDE,
    GREEN_AMPLITUDE
};

class SoundAnimator : public MatrixTask {
public:
    SoundAnimator(LedMatrix& matrix, AudioAnalyzer& analyzer);

    // Методы управления анимацией
    void setColorAmplitudeAnimation();
    void setGreenAmplitudeAnimation();
    void update();

    // Управление задачей
    void startTask() override;
    void stopTask() override;

    // Получение доступа к анализатору (по запросу извне)
    AudioAnalyzer& getAnalyzer();

private:
    LedMatrix& ledMatrix;
    AudioAnalyzer& audioAnalyzer;  // исправлено: теперь это ссылка

    unsigned long lastUpdateTime = 0;
    bool isAnimating = false;
    AnimationType currentAnimation = COLOR_AMPLITUDE;
    std::function<CRGB(uint8_t)> currentColorFunc;

    // Общий метод визуализации
    void renderAmplitude(std::function<CRGB(uint8_t)> colorFunc);

    // Задача анимации
    static void animationTask(void* param);
    TaskHandle_t animationTaskHandle = nullptr;
};

#endif // SOUND_ANIMATOR_HPP
