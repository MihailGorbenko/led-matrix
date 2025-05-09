#include "sound_animator.hpp"
#include "config.hpp"

// Конструктор
SoundAnimator::SoundAnimator(LedMatrix& matrix, AudioAnalyzer& analyzer)
    : ledMatrix(matrix),
      audioAnalyzer(analyzer),
      lastUpdateTime(0),
      isAnimating(false),
      currentAnimation(NONE),
      animationTaskHandle(nullptr)
{
}

// Визуализация: цветная амплитуда (по высоте)
void SoundAnimator::renderColorAmplitude() {
    audioAnalyzer.processAudio(MIC_PIN);

    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_WIDTH);

    CRGB* leds = ledMatrix.getLeds();
    ledMatrix.clear();

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = max(0, MATRIX_HEIGHT - heights[x]); y < MATRIX_HEIGHT; y++) {
            uint8_t hue = map(heights[x], 0, MATRIX_HEIGHT, 0, 255);
            leds[ledMatrix.XY(x, y)] = CHSV(hue, 255, 255);
        }
    }

    ledMatrix.update();
}

// Визуализация: зелёная амплитуда
void SoundAnimator::renderGreenAmplitude() {
    audioAnalyzer.processAudio(MIC_PIN);

    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_WIDTH);

    CRGB* leds = ledMatrix.getLeds();
    ledMatrix.clear();

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = max(0, MATRIX_HEIGHT - heights[x]); y < MATRIX_HEIGHT; y++) {
            leds[ledMatrix.XY(x, y)] = CRGB::Green;
        }
    }

    ledMatrix.update();
}

// Установка цветной анимации
void SoundAnimator::setColorAmplitudeAnimation() {
    isAnimating = true;
    currentAnimation = COLOR_AMPLITUDE;
}

// Установка зелёной анимации
void SoundAnimator::setGreenAmplitudeAnimation() {
    isAnimating = true;
    currentAnimation = GREEN_AMPLITUDE;
}

// Обновление визуализации
void SoundAnimator::update() {
    if (!isAnimating) return;

    switch (currentAnimation) {
        case COLOR_AMPLITUDE:
            renderColorAmplitude();
            break;
        case GREEN_AMPLITUDE:
            renderGreenAmplitude();
            break;
        default:
            break;
    }
}

// Задача обновления
void SoundAnimator::animationTask(void* param) {
    SoundAnimator* animator = static_cast<SoundAnimator*>(param);
    while (animator->isAnimating) {
        animator->update();
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
    vTaskDelete(nullptr); // Завершаем задачу корректно
}

// Запуск задачи
void SoundAnimator::startTask() {
    if (animationTaskHandle == nullptr) {
        Serial.println("[SoundAnimator] Starting animation task...");
        isAnimating = true;
        xTaskCreatePinnedToCore(
            animationTask,
            "AnimationTask",
            2048,
            this,
            1,
            &animationTaskHandle,
            1
        );
        Serial.println("[SoundAnimator] Animation task started.");
    } else {
        Serial.println("[SoundAnimator] Animation task is already running.");
    }
}

// Остановка задачи
void SoundAnimator::stopTask() {
    if (animationTaskHandle != nullptr) {
        Serial.println("[SoundAnimator] Stopping animation task...");
        isAnimating = false; // Позволим задаче выйти корректно
        animationTaskHandle = nullptr;

        ledMatrix.clear();
        ledMatrix.update();

        Serial.println("[SoundAnimator] Animation task stopped.");
    } else {
        Serial.println("[SoundAnimator] No animation task to stop.");
    }
}
