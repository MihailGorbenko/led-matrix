#ifndef SOUND_ANIMATOR_HPP
#define SOUND_ANIMATOR_HPP

#include <functional>
#include "led_matrix.hpp"
#include "audio_analyzer.hpp"
#include "matrix_task.hpp"
#include "async_serial.hpp"

enum AnimationType {
    COLOR_AMPLITUDE,
    GREEN_AMPLITUDE
};

class SoundAnimator : public MatrixTask {
private:
    LedMatrix& ledMatrix;
    AsyncSerial& serial;
    AudioAnalyzer audioAnalyzer; // Объект AudioAnalyzer создаётся внутри класса

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
    SoundAnimator(LedMatrix& matrix, AsyncSerial& asyncSerial, AudioAnalyzer& audioAnalyzer);

    // Методы управления анимацией
    void setColorAmplitudeAnimation();
    void setGreenAmplitudeAnimation();
    void update();

    // Реализация методов MatrixTask
    void startTask() override;
    void stopTask() override;


};

#endif // SOUND_ANIMATOR_HPP