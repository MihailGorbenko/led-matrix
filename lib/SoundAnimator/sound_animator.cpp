#include "sound_animator.hpp"
#include "config.hpp"
#include <cmath>
#include <Arduino.h>
#include <Preferences.h>

// Константы (объявления)
constexpr const char* NVS_NAMESPACE = "soundanim";
constexpr const char* KEY_COLOR_SENS = "COLOR_SENS";
constexpr const char* KEY_RECT_SENS = "RECT_SENS";
constexpr const char* KEY_SKY_SENS = "SKY_SENS";
constexpr const char* KEY_WAVE_SENS = "WAVE_SENS";
constexpr const char* KEY_STAR_MAX = "STAR_MAX";
constexpr const char* KEY_STAR_MIN_BRI = "STAR_MIN_BRI";
constexpr const char* KEY_STAR_MAX_BRI = "STAR_MAX_BRI";
constexpr const char* KEY_FADE_AMT = "FADE_AMT";
constexpr const char* KEY_WAVE_PHASE = "WAVE_PHASE";
constexpr const char* KEY_WAVE_FREQ = "WAVE_FREQ";
constexpr const char* KEY_RECT_MIN = "RECT_MIN";

constexpr float DEFAULT_COLOR_AMPLITUDE_SENSITIVITY = 1.5f;
constexpr float DEFAULT_PULSING_RECTANGLE_SENSITIVITY = 0.9f;
constexpr float DEFAULT_STARRY_SKY_SENSITIVITY = 0.8f;
constexpr float DEFAULT_WAVE_SENSITIVITY = 1.0f;
constexpr uint8_t DEFAULT_STAR_MAX_COUNT = 20;
constexpr uint8_t DEFAULT_STAR_MIN_BRIGHTNESS = 50;
constexpr uint8_t DEFAULT_STAR_MAX_BRIGHTNESS = 255;
constexpr uint8_t DEFAULT_FADE_AMOUNT = 200;
constexpr float DEFAULT_WAVE_PHASE_INCREMENT = 0.1f;
constexpr float DEFAULT_WAVE_FREQUENCY = 0.3f;
constexpr uint8_t DEFAULT_RECTANGLE_MIN_SIZE = 1;

SoundAnimator::SoundAnimator(LedMatrix& matrix)
    : ledMatrix(matrix),
      audioAnalyzer(),
      preferences(),
      colorAmplitudeSensitivity(DEFAULT_COLOR_AMPLITUDE_SENSITIVITY),
      pulsingRectangleSensitivity(DEFAULT_PULSING_RECTANGLE_SENSITIVITY),
      starrySkySensitivity(DEFAULT_STARRY_SKY_SENSITIVITY),
      waveSensitivity(DEFAULT_WAVE_SENSITIVITY),
      starrySkyMaxStars(DEFAULT_STAR_MAX_COUNT),
      starrySkyMinBrightness(DEFAULT_STAR_MIN_BRIGHTNESS),
      starrySkyMaxBrightness(DEFAULT_STAR_MAX_BRIGHTNESS),
      fadeAmount(DEFAULT_FADE_AMOUNT),
      wavePhaseIncrement(DEFAULT_WAVE_PHASE_INCREMENT),
      waveFrequency(DEFAULT_WAVE_FREQUENCY),
      rectangleMinSize(DEFAULT_RECTANGLE_MIN_SIZE),
      isAnimating(false),
      currentRenderMethod(nullptr),
      animationTaskHandle(nullptr) {
   
}

void SoundAnimator::init() {
    Serial.println("[SoundAnimator] Initializing...");
    preferences.begin(NVS_NAMESPACE, false);
    Serial.println("[SoundAnimator] Loading settings from NVS...");
    loadSettings();
    preferences.end();
    Serial.println("[SoundAnimator] Initialization complete.");
}

SoundAnimator::~SoundAnimator() {
    // Останавливаем задачу анимации, если она запущена
    stopTask();

    // Очищаем матрицу светодиодов
    ledMatrix.clear();
    ledMatrix.update();

    // Завершаем работу с NVS
    preferences.end();

    Serial.println("[SoundAnimator] Destructor called. Resources cleaned up.");
}

// ======================
//    NVS: загрузка
// ======================
void SoundAnimator::loadSettings() {
    if (!preferences.isKey(KEY_COLOR_SENS)) {
        preferences.putFloat(KEY_COLOR_SENS, colorAmplitudeSensitivity);
    } else {
        colorAmplitudeSensitivity = preferences.getFloat(KEY_COLOR_SENS, DEFAULT_COLOR_AMPLITUDE_SENSITIVITY);
    }
    Serial.printf("[SoundAnimator] colorAmpSens = %.2f\n", colorAmplitudeSensitivity);

    if (!preferences.isKey(KEY_RECT_SENS)) {
        preferences.putFloat(KEY_RECT_SENS, pulsingRectangleSensitivity);
    } else {
        pulsingRectangleSensitivity = preferences.getFloat(KEY_RECT_SENS, DEFAULT_PULSING_RECTANGLE_SENSITIVITY);
    }
    Serial.printf("[SoundAnimator] pulseRectSens = %.2f\n", pulsingRectangleSensitivity);

    if (!preferences.isKey(KEY_SKY_SENS)) {
        preferences.putFloat(KEY_SKY_SENS, starrySkySensitivity);
    } else {
        starrySkySensitivity = preferences.getFloat(KEY_SKY_SENS, DEFAULT_STARRY_SKY_SENSITIVITY);
    }
    Serial.printf("[SoundAnimator] starrySens     = %.2f\n", starrySkySensitivity);

    if (!preferences.isKey(KEY_WAVE_SENS)) {
        preferences.putFloat(KEY_WAVE_SENS, waveSensitivity);
    } else {
        waveSensitivity = preferences.getFloat(KEY_WAVE_SENS, DEFAULT_WAVE_SENSITIVITY);
    }
    Serial.printf("[SoundAnimator] waveSens       = %.2f\n", waveSensitivity);

    if (!preferences.isKey(KEY_STAR_MAX)) {
        preferences.putUChar(KEY_STAR_MAX, starrySkyMaxStars);
    } else {
        starrySkyMaxStars = preferences.getUChar(KEY_STAR_MAX, DEFAULT_STAR_MAX_COUNT);
    }
    Serial.printf("[SoundAnimator] starMax        = %u\n", starrySkyMaxStars);

    if (!preferences.isKey(KEY_STAR_MIN_BRI)) {
        preferences.putUChar(KEY_STAR_MIN_BRI, starrySkyMinBrightness);
    } else {
        starrySkyMinBrightness = preferences.getUChar(KEY_STAR_MIN_BRI, DEFAULT_STAR_MIN_BRIGHTNESS);
    }
    Serial.printf("[SoundAnimator] starMinB       = %u\n", starrySkyMinBrightness);

    if (!preferences.isKey(KEY_STAR_MAX_BRI)) {
        preferences.putUChar(KEY_STAR_MAX_BRI, starrySkyMaxBrightness);
    } else {
        starrySkyMaxBrightness = preferences.getUChar(KEY_STAR_MAX_BRI, DEFAULT_STAR_MAX_BRIGHTNESS);
    }
    Serial.printf("[SoundAnimator] starMaxB       = %u\n", starrySkyMaxBrightness);

    if (!preferences.isKey(KEY_FADE_AMT)) {
        preferences.putUChar(KEY_FADE_AMT, fadeAmount);
    } else {
        fadeAmount = preferences.getUChar(KEY_FADE_AMT, DEFAULT_FADE_AMOUNT);
    }
    Serial.printf("[SoundAnimator] fadeAmt        = %u\n", fadeAmount);

    if (!preferences.isKey(KEY_WAVE_PHASE)) {
        preferences.putFloat(KEY_WAVE_PHASE, wavePhaseIncrement);
    } else {
        wavePhaseIncrement = preferences.getFloat(KEY_WAVE_PHASE, DEFAULT_WAVE_PHASE_INCREMENT);
    }
    Serial.printf("[SoundAnimator] wavePhase      = %.2f\n", wavePhaseIncrement);

    if (!preferences.isKey(KEY_WAVE_FREQ)) {
        preferences.putFloat(KEY_WAVE_FREQ, waveFrequency);
    } else {
        waveFrequency = preferences.getFloat(KEY_WAVE_FREQ, DEFAULT_WAVE_FREQUENCY);
    }
    Serial.printf("[SoundAnimator] waveFreq       = %.2f\n", waveFrequency);

    if (!preferences.isKey(KEY_RECT_MIN)) {
        preferences.putUChar(KEY_RECT_MIN, rectangleMinSize);
    } else {
        rectangleMinSize = preferences.getUChar(KEY_RECT_MIN, DEFAULT_RECTANGLE_MIN_SIZE);
    }
    Serial.printf("[SoundAnimator] rectMin        = %u\n", rectangleMinSize);
}

// ======================
//    NVS: сохранение
// ======================
void SoundAnimator::saveSetting(const char* key, float value) {
    preferences.begin(NVS_NAMESPACE, false);
    preferences.putFloat(key, value);
    preferences.end();
    Serial.printf("[SoundAnimator] Saved %s = %.2f\n", key, value);
}

void SoundAnimator::saveSetting(const char* key, uint8_t value) {
    preferences.begin(NVS_NAMESPACE, false);
    preferences.putUChar(key, value);
    preferences.end();
    Serial.printf("[SoundAnimator] Saved %s = %u\n", key, value);
}

// Сброс всех настроек на дефолты
void SoundAnimator::resetSettings() {
    preferences.begin(NVS_NAMESPACE, false);
    preferences.clear(); // Очищаем все сохранённые настройки

    // Сохраняем значения по умолчанию в NVS
    preferences.putFloat(KEY_COLOR_SENS, DEFAULT_COLOR_AMPLITUDE_SENSITIVITY);
    preferences.putFloat(KEY_RECT_SENS, DEFAULT_PULSING_RECTANGLE_SENSITIVITY);
    preferences.putFloat(KEY_SKY_SENS, DEFAULT_STARRY_SKY_SENSITIVITY);
    preferences.putFloat(KEY_WAVE_SENS, DEFAULT_WAVE_SENSITIVITY);
    preferences.putUChar(KEY_STAR_MAX, DEFAULT_STAR_MAX_COUNT);
    preferences.putUChar(KEY_STAR_MIN_BRI, DEFAULT_STAR_MIN_BRIGHTNESS);
    preferences.putUChar(KEY_STAR_MAX_BRI, DEFAULT_STAR_MAX_BRIGHTNESS);
    preferences.putUChar(KEY_FADE_AMT, DEFAULT_FADE_AMOUNT);
    preferences.putFloat(KEY_WAVE_PHASE, DEFAULT_WAVE_PHASE_INCREMENT);
    preferences.putFloat(KEY_WAVE_FREQ, DEFAULT_WAVE_FREQUENCY);
    preferences.putUChar(KEY_RECT_MIN, DEFAULT_RECTANGLE_MIN_SIZE);

    preferences.end();

    // Обновляем переменные в памяти
    colorAmplitudeSensitivity = DEFAULT_COLOR_AMPLITUDE_SENSITIVITY;
    pulsingRectangleSensitivity = DEFAULT_PULSING_RECTANGLE_SENSITIVITY;
    starrySkySensitivity = DEFAULT_STARRY_SKY_SENSITIVITY;
    waveSensitivity = DEFAULT_WAVE_SENSITIVITY;
    starrySkyMaxStars = DEFAULT_STAR_MAX_COUNT;
    starrySkyMinBrightness = DEFAULT_STAR_MIN_BRIGHTNESS;
    starrySkyMaxBrightness = DEFAULT_STAR_MAX_BRIGHTNESS;
    fadeAmount = DEFAULT_FADE_AMOUNT;
    wavePhaseIncrement = DEFAULT_WAVE_PHASE_INCREMENT;
    waveFrequency = DEFAULT_WAVE_FREQUENCY;
    rectangleMinSize = DEFAULT_RECTANGLE_MIN_SIZE;
}

// ======================
// Сеттеры с валидацией
// ======================
void SoundAnimator::setColorAmplitudeSensitivity(float v) {
    if (v > 0.0f && v <= 10.0f) {
        colorAmplitudeSensitivity = v;
        saveSetting(KEY_COLOR_SENS, v);
    }
}
void SoundAnimator::setPulsingRectangleSensitivity(float v) {
    if (v > 0.0f && v <= 10.0f) {
        pulsingRectangleSensitivity = v;
        saveSetting(KEY_RECT_SENS, v);
    }
}
void SoundAnimator::setStarrySkySensitivity(float v) {
    if (v > 0.0f && v <= 10.0f) {
        starrySkySensitivity = v;
        saveSetting(KEY_SKY_SENS, v);
    }
}
void SoundAnimator::setWaveSensitivity(float v) {
    if (v > 0.0f && v <= 10.0f) {
        waveSensitivity = v;
        saveSetting(KEY_WAVE_SENS, v);
    }
}
void SoundAnimator::setStarrySkyMaxStars(uint8_t v) {
    starrySkyMaxStars = constrain(v, 1, MATRIX_WIDTH * MATRIX_HEIGHT);
    saveSetting(KEY_STAR_MAX, v);
}
void SoundAnimator::setStarrySkyMinBrightness(uint8_t v) {
    starrySkyMinBrightness = constrain(v, 0, 255);
    saveSetting(KEY_STAR_MIN_BRI, v);
}
void SoundAnimator::setStarrySkyMaxBrightness(uint8_t v) {
    starrySkyMaxBrightness = constrain(v, 0, 255);
    saveSetting(KEY_STAR_MAX_BRI, v);
}
void SoundAnimator::setFadeAmount(uint8_t v) {
    fadeAmount = constrain(v, 0, 255);
    saveSetting(KEY_FADE_AMT, v);
}
void SoundAnimator::setWavePhaseIncrement(float v) {
    if (v > 0.0f && v <= 1.0f) {
        wavePhaseIncrement = v;
        saveSetting(KEY_WAVE_PHASE, v);
    }
}
void SoundAnimator::setWaveFrequency(float v) {
    if (v > 0.0f && v <= 5.0f) {
        waveFrequency = v;
        saveSetting(KEY_WAVE_FREQ, v);
    }
}
void SoundAnimator::setRectangleMinSize(uint8_t v) {
    rectangleMinSize = constrain(v, 1, MATRIX_WIDTH);
    saveSetting(KEY_RECT_MIN, v);
}

// ==============
// Методы рендеринга
// ==============
void SoundAnimator::renderColorAmplitude(CRGB color) {
    audioAnalyzer.processAudio();
    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_WIDTH);

    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = MATRIX_HEIGHT - heights[x]; y < MATRIX_HEIGHT; y++) {
            if (color == CRGB::Black) {
                uint8_t hue = map(heights[x], 0, MATRIX_HEIGHT, 0, 255);
                leds[ledMatrix.XY(x, y)] = CHSV(hue, 255, 255);
            } else {
                leds[ledMatrix.XY(x, y)] = color;
            }
        }
    }
    ledMatrix.update(); // Заменено FastLED.show()
}

void SoundAnimator::renderPulsingRectangle(CRGB color) {
    // Обрабатываем аудиосигнал
    audioAnalyzer.processAudio();
    float logRmsEnergy = audioAnalyzer.getTotalLogRmsEnergy(); // Получаем логарифмическую RMS-энергию
    float minLogPower = audioAnalyzer.getMinLogPower();        // Минимальное значение мощности
    float maxLogPower = audioAnalyzer.getMaxLogPower();        // Максимальное значение мощности

    // Усиление сигнала с учётом чувствительности
    float amplified = logRmsEnergy * pulsingRectangleSensitivity;

    // Используем статистику для определения диапазона
    float dynamicMinLogPower = minLogPower;
    float dynamicMaxLogPower = maxLogPower;

    // Убедимся, что диапазон корректен
    dynamicMinLogPower = constrain(dynamicMinLogPower, 1.0f, 50.0f);
    dynamicMaxLogPower = constrain(dynamicMaxLogPower, dynamicMinLogPower + 1.0f, 100.0f);

    // Вычисляем размеры прямоугольника
    uint8_t w = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, rectangleMinSize, MATRIX_WIDTH);
    uint8_t h = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, rectangleMinSize, MATRIX_HEIGHT);

    // Ограничиваем размеры
    w = constrain(w, rectangleMinSize, MATRIX_WIDTH);
    h = constrain(h, rectangleMinSize, MATRIX_HEIGHT);

    // Получаем массив светодиодов
    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    // Вычисляем координаты прямоугольника
    int cx = MATRIX_WIDTH / 2, cy = MATRIX_HEIGHT / 2;
    int sx = cx - w / 2, sy = cy - h / 2, ex = cx + w / 2 - 1, ey = cy + h / 2 - 1;

    // Ограничиваем координаты
    sx = constrain(sx, 0, MATRIX_WIDTH - 1);
    sy = constrain(sy, 0, MATRIX_HEIGHT - 1);
    ex = constrain(ex, 0, MATRIX_WIDTH - 1);
    ey = constrain(ey, 0, MATRIX_HEIGHT - 1);

    // Рисуем прямоугольник
    for (int x = sx; x <= ex; x++) {
        leds[ledMatrix.XY(x, sy)] = color;
        leds[ledMatrix.XY(x, ey)] = color;
    }
    for (int y = sy; y <= ey; y++) {
        leds[ledMatrix.XY(sx, y)] = color;
        leds[ledMatrix.XY(ex, y)] = color;
    }

    // Обновляем матрицу
    ledMatrix.update();
}

void SoundAnimator::renderStarrySky(CRGB color) {
    // Обрабатываем аудиосигнал
    audioAnalyzer.processAudio();
    float logRmsEnergy = audioAnalyzer.getTotalLogRmsEnergy(); // Получаем логарифмическую RMS-энергию
    float minLogPower = audioAnalyzer.getMinLogPower();        // Минимальное значение мощности
    float maxLogPower = audioAnalyzer.getMaxLogPower();        // Максимальное значение мощности

    // Усиление сигнала с учётом чувствительности
    float amplified = logRmsEnergy * starrySkySensitivity;

    // Используем статистику для определения диапазона
    float dynamicMinLogPower = minLogPower;
    float dynamicMaxLogPower = maxLogPower;

    // Убедимся, что диапазон корректен
    dynamicMinLogPower = constrain(dynamicMinLogPower, 1.0f, 50.0f);
    dynamicMaxLogPower = constrain(dynamicMaxLogPower, dynamicMinLogPower + 1.0f, 100.0f);

    // Вычисляем количество звёзд
    uint8_t count = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, 1, starrySkyMaxStars);
    count = constrain(count, 1, starrySkyMaxStars);

    // Очищаем матрицу с эффектом затухания
    CRGB* leds = ledMatrix.getLeds();
    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
        leds[i].nscale8(fadeAmount);
    }

    // Рисуем звёзды
    for (int i = 0; i < count; i++) {
        int x = random(0, MATRIX_WIDTH);
        int y = random(0, MATRIX_HEIGHT);
        uint8_t brightness = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, starrySkyMinBrightness, starrySkyMaxBrightness);
        brightness = constrain(brightness, starrySkyMinBrightness, starrySkyMaxBrightness);
        leds[ledMatrix.XY(x, y)] = color.nscale8(brightness);
    }

    // Обновляем матрицу
    ledMatrix.update();
}

void SoundAnimator::renderWave(CRGB color) {
    // Обрабатываем аудиосигнал
    audioAnalyzer.processAudio();
    float logRmsEnergy = audioAnalyzer.getTotalLogRmsEnergy(); // Получаем логарифмическую RMS-энергию
    float minLogPower = audioAnalyzer.getMinLogPower();        // Минимальное значение мощности
    float maxLogPower = audioAnalyzer.getMaxLogPower();        // Максимальное значение мощности

    // Усиление сигнала с учётом чувствительности
    float amplified = logRmsEnergy * waveSensitivity;

    // Используем статистику для определения диапазона
    float dynamicMinLogPower = minLogPower;
    float dynamicMaxLogPower = maxLogPower;

    // Убедимся, что диапазон корректен
    dynamicMinLogPower = constrain(dynamicMinLogPower, 1.0f, 50.0f);
    dynamicMaxLogPower = constrain(dynamicMaxLogPower, dynamicMinLogPower + 1.0f, 100.0f);

    // Вычисляем высоту волны
    uint8_t waveH = map(amplified, dynamicMinLogPower, dynamicMaxLogPower, 1, MATRIX_HEIGHT / 2);
    waveH = constrain(waveH, 1, MATRIX_HEIGHT / 2);

    // Фаза волны
    static float phase = 0;
    phase += wavePhaseIncrement;
    phase = fmod(phase, 2 * PI); // Ограничиваем phase

    // Очищаем матрицу
    CRGB* leds = ledMatrix.getLeds();
    fill_solid(leds, MATRIX_WIDTH * MATRIX_HEIGHT, CRGB::Black);

    // Рисуем волну
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        int cy = MATRIX_HEIGHT / 2;
        int wy = cy + sin(phase + x * waveFrequency) * waveH;
        wy = constrain(wy, 0, MATRIX_HEIGHT - 1);
        leds[ledMatrix.XY(x, wy)] = color;

        // Отражённая волна
        int my = cy - (wy - cy);
        my = constrain(my, 0, MATRIX_HEIGHT - 1);
        leds[ledMatrix.XY(x, my)] = color;
    }

    // Обновляем матрицу
    ledMatrix.update();
}

// ======================
// Универсальный селектор анимации
// ======================
void SoundAnimator::setAnimation(AnimationType type, CRGB color) {
    isAnimating = true;
    switch (type) {
        case AnimationType::ColorAmplitude:
            currentRenderMethod = [this, color]() { renderColorAmplitude(color); };
            break;
        case AnimationType::PulsingRectangle:
            currentRenderMethod = [this, color]() { renderPulsingRectangle(color); };
            break;
        case AnimationType::StarrySky:
            currentRenderMethod = [this, color]() { renderStarrySky(color); };
            break;
        case AnimationType::Wave:
            currentRenderMethod = [this, color]() { renderWave(color); };
            break;
        default:
            Serial.println("[SoundAnimator] Unsupported animation type!");
            currentRenderMethod = nullptr;
            isAnimating = false;
            break;
    }
}

// Обновление кадра
void SoundAnimator::update() {
    if(isAnimating && currentRenderMethod) currentRenderMethod();
}

// Задача FreeRTOS
void SoundAnimator::animationTask(void* param) {
    SoundAnimator* s = static_cast<SoundAnimator*>(param);
    while(s->isAnimating) {
        s->update();
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
    s->animationTaskHandle = nullptr;
    vTaskDelete(nullptr);
}

void SoundAnimator::initializeAudioAnalyzer() {
    audioAnalyzer.begin();
}

void SoundAnimator::startTask() {
    if(!animationTaskHandle) {
        isAnimating = true;
        xTaskCreatePinnedToCore(animationTask, "AnimTask", 4096, this, 1, &animationTaskHandle, 1);
    }
}

void SoundAnimator::stopTask() {
    if (animationTaskHandle) {
        isAnimating = false;
        unsigned long startTime = millis();
        while (animationTaskHandle) {
            vTaskDelay(pdMS_TO_TICKS(10));
            if (millis() - startTime > 1000) { // Тайм-аут 1 секунда
                Serial.println("[SoundAnimator] Task stop timeout!");
                break;
            }
        }
        ledMatrix.clear();
        ledMatrix.update();
    }
}

AudioAnalyzer& SoundAnimator::getAudioAnalyzer() {
    return audioAnalyzer;
}