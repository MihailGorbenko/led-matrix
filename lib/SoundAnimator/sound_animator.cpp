#include "sound_animator.hpp"
#include "config.hpp"
#include <cmath>
#include <functional> // Для std::function

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

// Пульсирующий прямоугольник
void SoundAnimator::renderPulsingRectangle(CRGB color) {
    audioAnalyzer.processAudio();
    float avgLogPower = audioAnalyzer.getTotalLogRmsEnergy(); // Используем новый метод

    // Усиливаем мощность для повышения чувствительности
    float amplifiedLogPower = avgLogPower * 1.1f; // Увеличиваем чувствительность

    // Преобразуем логарифмическую мощность в размеры прямоугольника
    uint8_t rectWidth = map(amplifiedLogPower, 10, 30, 1, MATRIX_WIDTH);
    uint8_t rectHeight = map(amplifiedLogPower, 10, 30, 1, MATRIX_HEIGHT);

    rectWidth = constrain(rectWidth, 1, MATRIX_WIDTH);
    rectHeight = constrain(rectHeight, 1, MATRIX_HEIGHT);

    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    // Вычисляем координаты прямоугольника
    int centerX = MATRIX_WIDTH / 2;
    int centerY = (MATRIX_HEIGHT / 2); // Смещаем центр на одну строку выше

    int startX = centerX - rectWidth / 2;
    int startY = centerY - rectHeight / 2;
    int endX = centerX + rectWidth / 2 - 1;
    int endY = centerY + rectHeight / 2 - 1;

    // Рисуем только границы прямоугольника
    for (int x = startX; x <= endX; x++) {
        leds[ledMatrix.XY(x, startY)] = color; // Верхняя граница
        leds[ledMatrix.XY(x, endY)] = color;   // Нижняя граница
    }

    for (int y = startY; y <= endY; y++) {
        leds[ledMatrix.XY(startX, y)] = color; // Левая граница
        leds[ledMatrix.XY(endX, y)] = color;   // Правая граница
    }

    FastLED.show();
}

// Звёздное небо
void SoundAnimator::renderStarrySky(CRGB color) {
    audioAnalyzer.processAudio();
    float avgLogPower = audioAnalyzer.getTotalLogRmsEnergy(); // Используем новый метод

    // Усиливаем мощность для повышения чувствительности
    float amplifiedLogPower = avgLogPower * 1.5f; // Увеличиваем чувствительность

    // Количество звёзд зависит от мощности звука
    uint8_t starCount = map(amplifiedLogPower, 10, 35, 1, 20);
    starCount = constrain(starCount, 1, 20);

    CRGB* leds = ledMatrix.getLeds();

    // Добавляем эффект "следа"
    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
        leds[i].nscale8(200); // Затухание предыдущего кадра
    }

    // Рисуем звёзды
    for (int i = 0; i < starCount; i++) {
        int x = random(0, MATRIX_WIDTH);
        int y = random(0, MATRIX_HEIGHT);

        // Яркость звезды зависит от мощности звука
        uint8_t brightness = map(amplifiedLogPower, 10, 35, 50, 255);
        brightness = constrain(brightness, 50, 255);

        leds[ledMatrix.XY(x, y)] = color.nscale8(brightness); // Цвет звезды с яркостью
    }

    FastLED.show();
}

// Волна
void SoundAnimator::renderWave(CRGB color) {
    audioAnalyzer.processAudio();
    float avgLogPower = audioAnalyzer.getTotalLogRmsEnergy(); // Используем новый метод

    // Усиливаем мощность для повышения чувствительности
    float amplifiedLogPower = avgLogPower * 1.2;

    // Высота волны зависит от мощности звука
    uint8_t waveHeight = map(amplifiedLogPower, 10, 35, 1, MATRIX_HEIGHT / 2);
    waveHeight = constrain(waveHeight, 1, MATRIX_HEIGHT / 2);

    // Смещение по фазе для анимации
    static float phase = 0.0;
    phase += 0.1; // Скорость движения волны

    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    // Рисуем основную волну
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        // Вычисляем вертикальную позицию основной волны
        int centerY = MATRIX_HEIGHT / 2;
        int waveY = centerY + sin(phase + x * 0.3) * waveHeight;

        // Ограничиваем координаты волны
        waveY = constrain(waveY, 0, MATRIX_HEIGHT - 1);

        // Рисуем пиксель основной волны
        leds[ledMatrix.XY(x, waveY)] = color;

        // Вычисляем вертикальную позицию зеркальной волны
        int mirroredWaveY = centerY - (waveY - centerY);

        // Ограничиваем координаты зеркальной волны
        mirroredWaveY = constrain(mirroredWaveY, 0, MATRIX_HEIGHT - 1);

        // Рисуем пиксель зеркальной волны
        leds[ledMatrix.XY(x, mirroredWaveY)] = color;
    }

    FastLED.show();
}

// Методы выбора анимации
void SoundAnimator::setColorAmplitudeAnimation() {
    isAnimating = true;
    currentRenderMethod = [this]() { renderColorAmplitude(); };
}

void SoundAnimator::setGreenAmplitudeAnimation() {
    isAnimating = true;
    currentRenderMethod = [this]() { renderGreenAmplitude(); };
}

void SoundAnimator::setPulsingRectangleAnimation(CRGB color) {
    isAnimating = true;
    currentRenderMethod = [this, color]() { renderPulsingRectangle(color); };
}

void SoundAnimator::setStarrySkyAnimation(CRGB color) {
    isAnimating = true;
    currentRenderMethod = [this, color]() { renderStarrySky(color); };
}

void SoundAnimator::setWaveAnimation(CRGB color) {
    isAnimating = true;
    currentRenderMethod = [this, color]() { renderWave(color); };
}

// Обновление текущего кадра
void SoundAnimator::update() {
    if (!isAnimating || !currentRenderMethod) return;
    currentRenderMethod(); // Вызываем текущий метод рендера
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
