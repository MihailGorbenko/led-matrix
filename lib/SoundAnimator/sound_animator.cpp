#include "sound_animator.hpp"
#include "config.hpp"

// Конструктор
SoundAnimator::SoundAnimator(LedMatrix& matrix, AudioAnalyzer& analyzer)
    : ledMatrix(matrix),
      audioAnalyzer(analyzer),
      lastUpdateTime(0),
      isAnimating(false),
      currentAnimation(COLOR_AMPLITUDE),
      currentColorFunc(nullptr),
      animationTaskHandle(nullptr)
{

}

// Общий метод для визуализации амплитуды
void SoundAnimator::renderAmplitude(std::function<CRGB(uint8_t)> colorFunc) {
    if (!colorFunc) return;  // Защита от null-функции

    audioAnalyzer.processAudio(MIC_PIN);

    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_WIDTH);

    CRGB* leds = ledMatrix.getLeds();
    ledMatrix.clear();

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = max(0, MATRIX_HEIGHT - heights[x]); y < MATRIX_HEIGHT; y++) {
            leds[ledMatrix.XY(x, y)] = colorFunc(heights[x]);
        }
    }

    ledMatrix.update();
}

// Установка цветной визуализации
void SoundAnimator::setColorAmplitudeAnimation() {
    isAnimating = true;
    currentAnimation = COLOR_AMPLITUDE;
    currentColorFunc = [](uint8_t height) {
        uint8_t hue = map(height, 0, MATRIX_HEIGHT, 0, 255);
        return CHSV(hue, 255, 255);
    };
}

// Установка зелёной визуализации
void SoundAnimator::setGreenAmplitudeAnimation() {
    isAnimating = true;
    currentAnimation = GREEN_AMPLITUDE;
    currentColorFunc = [](uint8_t) {
        return CRGB::Green;
    };
}

// Обновление визуализации (FPS контролируется задачей)
void SoundAnimator::update() {
    if (!isAnimating || !currentColorFunc) return;

    renderAmplitude(currentColorFunc);
}

// Задача обновления анимации
void SoundAnimator::animationTask(void* param) {
    SoundAnimator* animator = static_cast<SoundAnimator*>(param);

    while (true) {
        animator->update();
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL)); // ~30 FPS
    }
}

// Запуск задачи
void SoundAnimator::startTask() {
    if (animationTaskHandle == nullptr) {
        Serial.println("[SoundAnimator] Starting animation task...");
        xTaskCreatePinnedToCore(
            animationTask,
            "AnimationTask",
            2048,
            this,
            1,
            &animationTaskHandle,
            1 // Ядро 1 — можно настроить
        );
        isAnimating = true;
        Serial.println("[SoundAnimator] Animation task started.");
    } else {
        Serial.println("[SoundAnimator] Animation task is already running.");
    }
}

// Остановка задачи
void SoundAnimator::stopTask() {
    if (animationTaskHandle != nullptr) {
        Serial.println("[SoundAnimator] Stopping animation task...");
        vTaskDelete(animationTaskHandle);
        animationTaskHandle = nullptr;
        isAnimating = false;

        ledMatrix.clear();  // Очищаем матрицу
        ledMatrix.update(); // Применяем изменения

        Serial.println("[SoundAnimator] Animation task stopped.");
    } else {
        Serial.println("[SoundAnimator] No animation task to stop.");
    }
}
