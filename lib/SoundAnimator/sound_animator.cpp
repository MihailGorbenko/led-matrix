#include "sound_animator.hpp"
#include "config.hpp"

// Конструктор
SoundAnimator::SoundAnimator(LedMatrix& matrix)
    : ledMatrix(matrix),
      audioAnalyzer(), // Инициализация AudioAnalyzer
      lastUpdateTime(0),
      isAnimating(false),
      currentRenderMethod(nullptr),
      animationTaskHandle(nullptr) {}

// Метод для получения ссылки на AudioAnalyzer
AudioAnalyzer& SoundAnimator::getAudioAnalyzer() {
    return audioAnalyzer;
}

// Визуализация: цветная амплитуда (по высоте)
void SoundAnimator::renderColorAmplitude() {
    audioAnalyzer.processAudio();

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
    audioAnalyzer.processAudio();

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

// Визуализация: пульсирующее кольцо
void SoundAnimator::renderPulsingRing() {
    audioAnalyzer.processAudio();

    // Получаем среднее значение логарифмической мощности
    float avgLogPower = audioAnalyzer.getSmoothedLogPower();

    // Выводим значение в Serial
    Serial.printf("[SoundAnimator] Smoothed Log Power: %.2f\n", avgLogPower);

    // Преобразуем логарифмическую мощность в радиус кольца
    int radius = map(avgLogPower, 20, 30, 0, MATRIX_WIDTH / 2); // Обновлён диапазон входных значений
    radius = constrain(radius, 0, MATRIX_WIDTH / 2); // Ограничиваем радиус

    Serial.printf("[SoundAnimator] Calculated Radius: %d\n", radius);

    CRGB* leds = ledMatrix.getLeds();
    ledMatrix.clear();

    // Отрисовка кольца
    for (int angle = 0; angle < 360; angle += 5) { // Шаг угла для плотности кольца
        int xPos = MATRIX_WIDTH / 2 + radius * cos(radians(angle));
        int yPos = MATRIX_HEIGHT / 2 + radius * sin(radians(angle));

        if (xPos >= 0 && xPos < MATRIX_WIDTH && yPos >= 0 && yPos < MATRIX_HEIGHT) {
            leds[ledMatrix.XY(xPos, yPos)] = CHSV(map(radius, 0, MATRIX_WIDTH / 2, 0, 255), 255, 255);
        }
    }

    ledMatrix.update();
}

// Установка цветной анимации
void SoundAnimator::setColorAmplitudeAnimation() {
    isAnimating = true;
    currentRenderMethod = &SoundAnimator::renderColorAmplitude; // Указываем метод рендера
}

// Установка зелёной анимации
void SoundAnimator::setGreenAmplitudeAnimation() {
    isAnimating = true;
    currentRenderMethod = &SoundAnimator::renderGreenAmplitude; // Указываем метод рендера
}

// Установка анимации пульсирующего кольца
void SoundAnimator::setPulsingRingAnimation() {
    isAnimating = true;
    currentRenderMethod = &SoundAnimator::renderPulsingRing; // Указываем метод рендера
}

// Обновление визуализации
void SoundAnimator::update() {
    if (!isAnimating || currentRenderMethod == nullptr) return;

    // Вызываем текущий метод рендера
    (this->*currentRenderMethod)();
}

// Задача обновления
void SoundAnimator::animationTask(void* param) {
    SoundAnimator* animator = static_cast<SoundAnimator*>(param);
    while (animator->isAnimating) {
        animator->update();
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
    animator->animationTaskHandle = nullptr; // Сбрасываем handle
    vTaskDelete(nullptr); // Завершаем задачу корректно
}

void SoundAnimator::initializeAudioAnalyzer() {
    audioAnalyzer.begin(); // Инициализация AudioAnalyzer
    
}

// Запуск задачи
void SoundAnimator::startTask() {
    
    if (animationTaskHandle == nullptr) {
        Serial.println("[SoundAnimator] Starting animation task...");
        isAnimating = true;
        xTaskCreatePinnedToCore(
            animationTask,
            "AnimationTask",
            4096, // Увеличьте размер стека, если требуется
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

        // Ожидаем завершения задачи
        while (animationTaskHandle != nullptr) {
            vTaskDelay(pdMS_TO_TICKS(10)); // Небольшая задержка для ожидания завершения
        }

        ledMatrix.clear();
        ledMatrix.update();

        Serial.println("[SoundAnimator] Animation task stopped.");
    } else {
        Serial.println("[SoundAnimator] No animation task to stop.");
    }
}
