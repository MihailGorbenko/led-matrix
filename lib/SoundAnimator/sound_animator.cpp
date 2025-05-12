#include "sound_animator.hpp"
#include "config.hpp"
#include <cmath>

// Конструктор
SoundAnimator::SoundAnimator(LedMatrix& matrix)
    : ledMatrix(matrix),
      audioAnalyzer(),
      lastUpdateTime(0),
      isAnimating(false),
      currentRenderMethod(nullptr),
      animationTaskHandle(nullptr) {}

// Получение ссылки на анализатор
AudioAnalyzer& SoundAnimator::getAudioAnalyzer() {
    return audioAnalyzer;
}

// Цветная амплитуда по полосам
void SoundAnimator::renderColorAmplitude() {
    audioAnalyzer.processAudio();

    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_WIDTH);

    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        uint8_t hue = map(heights[x], 0, MATRIX_HEIGHT, 0, 255);
        for (int y = MATRIX_HEIGHT - heights[x]; y < MATRIX_HEIGHT; y++) {
            leds[ledMatrix.XY(x, y)] = CHSV(hue, 255, 255);
        }
    }

    FastLED.show();
}

// Зелёная амплитуда
void SoundAnimator::renderGreenAmplitude() {
    audioAnalyzer.processAudio();

    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_WIDTH);

    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = MATRIX_HEIGHT - heights[x]; y < MATRIX_HEIGHT; y++) {
            leds[ledMatrix.XY(x, y)] = CRGB::Green;
        }
    }

    FastLED.show();
}

// Пульсирующее кольцо (оптимизировано)
void SoundAnimator::renderPulsingRing() {
    audioAnalyzer.processAudio();
    float avgLogPower = audioAnalyzer.getSmoothedLogPower();

    uint8_t radius = map(avgLogPower * 10, 150, 260, 1, MATRIX_WIDTH / 2);
    radius = constrain(radius, 1, MATRIX_WIDTH / 2);

    uint8_t hue = map(radius, 1, MATRIX_WIDTH / 2, 0, 255);
    CHSV color(hue, 255, 255);

    constexpr float aspectRatio = static_cast<float>(MATRIX_WIDTH) / MATRIX_HEIGHT;
    constexpr float centerX = MATRIX_WIDTH / 2.0f;
    constexpr float centerY = MATRIX_HEIGHT / 2.0f;

    constexpr uint8_t angleStep = 6;
    constexpr uint8_t numSteps = 360 / angleStep;

    static constexpr float radiansLUT[numSteps] = [] {
        float arr[numSteps] = {};
        for (int i = 0; i < numSteps; ++i)
            arr[i] = (i * angleStep) * PI / 180.0f;
        return arr;
    }();

    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    for (uint8_t i = 0; i < numSteps; ++i) {
        float angle = radiansLUT[i];

        int xi = roundf(centerX + radius * cosf(angle));
        int yi = roundf(centerY + radius * sinf(angle) * aspectRatio);

        if (xi >= 0 && xi < MATRIX_WIDTH && yi >= 0 && yi < MATRIX_HEIGHT) {
            leds[ledMatrix.XY(xi, yi)] = color;
        }
    }

    FastLED.show();
}

// Методы выбора анимации
void SoundAnimator::setColorAmplitudeAnimation() {
    isAnimating = true;
    currentRenderMethod = &SoundAnimator::renderColorAmplitude;
}

void SoundAnimator::setGreenAmplitudeAnimation() {
    isAnimating = true;
    currentRenderMethod = &SoundAnimator::renderGreenAmplitude;
}

void SoundAnimator::setPulsingRingAnimation() {
    isAnimating = true;
    currentRenderMethod = &SoundAnimator::renderPulsingRing;
}

// Обновление текущего кадра
void SoundAnimator::update() {
    if (!isAnimating || currentRenderMethod == nullptr) return;
    (this->*currentRenderMethod)();
}

// Задача для обновления анимации
void SoundAnimator::animationTask(void* param) {
    SoundAnimator* animator = static_cast<SoundAnimator*>(param);
    while (animator->isAnimating) {
        animator->update();
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
    animator->animationTaskHandle = nullptr;
    vTaskDelete(nullptr);
}

// Инициализация анализатора
void SoundAnimator::initializeAudioAnalyzer() {
    audioAnalyzer.begin();
}

// Запуск анимации в отдельной задаче
void SoundAnimator::startTask() {
    if (animationTaskHandle == nullptr) {
        isAnimating = true;
        xTaskCreatePinnedToCore(
            animationTask,
            "AnimationTask",
            4096,
            this,
            1,
            &animationTaskHandle,
            1
        );
    }
}

// Остановка анимации и задачи
void SoundAnimator::stopTask() {
    if (animationTaskHandle != nullptr) {
        isAnimating = false;
        while (animationTaskHandle != nullptr) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        ledMatrix.clear();
        ledMatrix.update();
    }
}
