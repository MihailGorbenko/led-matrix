#include "audio_analyzer.hpp"
#include <nvs_flash.h>
#include <cmath>
#include <Arduino.h>

// --- Конструктор ---
AudioAnalyzer::AudioAnalyzer()
    : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY)
{
    // Инициализация коэффициентов с дефолтными значениями
    sensitivityReduction = 5.0f; // Коэффициент для уменьшения чувствительности
    lowFreqGain = 1.0f;          // Усиление низких частот
    midFreqGain = 1.0f;          // Усиление средних частот
    highFreqGain = 1.0f;         // Усиление высоких частот
    alpha = 0.2f;                // Коэффициент для сглаживания аудиосигнала
    fMin = 50.0f;                // Минимальная частота для анализа
    fMax = 10000.0f;             // Максимальная частота для анализа
    noiseThresholdRatio = 0.25f; // Порог шума для определения амплитуды
    bandDecay = 0.95f;           // Коэффициент затухания для каждой полосы
    bandCeiling = 1000;          // Максимальное значение для каждой полосы

    memset(bands, 0, sizeof(bands));
    memset(smoothedBands, 0, sizeof(smoothedBands));
    Serial.println("[AudioAnalyzer] Constructor called.");
}

// --- Деструктор ---
AudioAnalyzer::~AudioAnalyzer() {
    preferences.end();  // Закрываем пространство имен
    Serial.println("[AudioAnalyzer] Preferences closed.");
}

// --- Инициализация ---
void AudioAnalyzer::begin() {
    Serial.println("[AudioAnalyzer] Initializing...");
    preferences.begin("audioanalyzer", true); // Открываем пространство имен
    loadSettings(); // Загружаем настройки из NVS
    preferences.end(); // Закрываем пространство имен
    Serial.println("[AudioAnalyzer] Initialization complete.");
}

// --- Загрузка настроек ---
void AudioAnalyzer::loadSettings() {
    sensitivityReduction = preferences.getFloat("sensReduct", 5.0f);
    lowFreqGain = preferences.getFloat("lowGain", 1.0f);
    midFreqGain = preferences.getFloat("midGain", 1.0f);
    highFreqGain = preferences.getFloat("highGain", 1.0f);
    alpha = preferences.getFloat("alpha", 0.2f);
    fMin = preferences.getFloat("fMin", 50.0f);
    fMax = preferences.getFloat("fMax", 10000.0f);
    noiseThresholdRatio = preferences.getFloat("nThresh", 0.25f);
    bandDecay = preferences.getFloat("bDecay", 0.95f);
    bandCeiling = preferences.getInt("bCeil", 1000);

    // Логирование загруженных значений
    Serial.printf("[AudioAnalyzer] Loaded sensitivityReduction: %.2f\n", sensitivityReduction);
    Serial.printf("[AudioAnalyzer] Loaded lowFreqGain: %.2f\n", lowFreqGain);
    Serial.printf("[AudioAnalyzer] Loaded midFreqGain: %.2f\n", midFreqGain);
    Serial.printf("[AudioAnalyzer] Loaded highFreqGain: %.2f\n", highFreqGain);
    Serial.printf("[AudioAnalyzer] Loaded alpha: %.2f\n", alpha);
    Serial.printf("[AudioAnalyzer] Loaded fMin: %.1f\n", fMin);
    Serial.printf("[AudioAnalyzer] Loaded fMax: %.1f\n", fMax);
    Serial.printf("[AudioAnalyzer] Loaded noiseThresholdRatio: %.2f\n", noiseThresholdRatio);
    Serial.printf("[AudioAnalyzer] Loaded bandDecay: %.3f\n", bandDecay);
    Serial.printf("[AudioAnalyzer] Loaded bandCeiling: %d\n", bandCeiling);
}

// --- Сохранение настроек ---
void AudioAnalyzer::saveSetting(const char* key, float value) {
    preferences.begin("audioanalyzer", false); // Открываем пространство имен для записи
    preferences.putFloat(key, value); // Сохраняем значение
    preferences.end(); // Закрываем пространство имен
    Serial.printf("[AudioAnalyzer] Saved %s: %.2f\n", key, value);
}

void AudioAnalyzer::saveSetting(const char* key, int value) {
    preferences.begin("audioanalyzer", false); // Открываем пространство имен для записи
    preferences.putInt(key, value); // Сохраняем значение
    preferences.end(); // Закрываем пространство имен
    Serial.printf("[AudioAnalyzer] Saved %s: %d\n", key, value);
}

// --- Методы для установки коэффициентов с проверками ---

// Устанавливает коэффициент чувствительности. Этот коэффициент влияет на то, насколько чувствительно устройство к изменениям звукового сигнала.
void AudioAnalyzer::setSensitivityReduction(float value) {
    if (value >= 0.1f && value <= 100.0f) {
        sensitivityReduction = value;
        saveSetting("sensReduct", sensitivityReduction);
        Serial.printf("[AudioAnalyzer] Set sensitivityReduction: %.2f\n", sensitivityReduction);
    } else {
        Serial.println("[AudioAnalyzer] Invalid sensitivityReduction value.");
    }
}

// Устанавливает усиление низких частот. Увеличение этого значения усиливает нижнюю часть спектра звука.
void AudioAnalyzer::setLowFreqGain(float value) {
    if (value >= 0.0f && value <= 10.0f) {
        lowFreqGain = value;
        saveSetting("lowGain", lowFreqGain);
        Serial.printf("[AudioAnalyzer] Set lowFreqGain: %.2f\n", lowFreqGain);
    } else {
        Serial.println("[AudioAnalyzer] Invalid lowFreqGain value.");
    }
}

// Устанавливает усиление средних частот. Влияет на частоты, которые обычно находятся в диапазоне человеческой речи и музыки.
void AudioAnalyzer::setMidFreqGain(float value) {
    if (value >= 0.0f && value <= 10.0f) {
        midFreqGain = value;
        saveSetting("midGain", midFreqGain);
        Serial.printf("[AudioAnalyzer] Set midFreqGain: %.2f\n", midFreqGain);
    } else {
        Serial.println("[AudioAnalyzer] Invalid midFreqGain value.");
    }
}

// Устанавливает усиление высоких частот. Увеличение этого значения сделает звук ярче, с акцентом на высокие частоты.
void AudioAnalyzer::setHighFreqGain(float value) {
    if (value >= 0.0f && value <= 10.0f) {
        highFreqGain = value;
        saveSetting("highGain", highFreqGain);
        Serial.printf("[AudioAnalyzer] Set highFreqGain: %.2f\n", highFreqGain);
    } else {
        Serial.println("[AudioAnalyzer] Invalid highFreqGain value.");
    }
}

// Устанавливает коэффициент сглаживания для аудиосигнала. Это значение контролирует, как быстро система реагирует на изменения в сигнале.
void AudioAnalyzer::setAlpha(float value) {
    if (value >= 0.01f && value <= 1.0f) {
        alpha = value;
        saveSetting("alpha", alpha);
        Serial.printf("[AudioAnalyzer] Set alpha: %.2f\n", alpha);
    } else {
        Serial.println("[AudioAnalyzer] Invalid alpha value.");
    }
}

// Устанавливает минимальную частоту для анализа спектра. Частоты ниже этого значения будут игнорироваться.
void AudioAnalyzer::setFMin(float value) {
    if (value >= 10.0f && value <= 1000.0f) {
        fMin = value;
        saveSetting("fMin", fMin);
        Serial.printf("[AudioAnalyzer] Set fMin: %.1f\n", fMin);
    } else {
        Serial.println("[AudioAnalyzer] Invalid fMin value.");
    }
}

// Устанавливает максимальную частоту для анализа спектра. Частоты выше этого значения будут игнорироваться.
void AudioAnalyzer::setFMax(float value) {
    if (value >= 1000.0f && value <= 30000.0f) {
        fMax = value;
        saveSetting("fMax", fMax);
        Serial.printf("[AudioAnalyzer] Set fMax: %.1f\n", fMax);
    } else {
        Serial.println("[AudioAnalyzer] Invalid fMax value.");
    }
}

// Устанавливает порог шума. Звук с амплитудой ниже этого значения считается фоном и не учитывается в анализе.
void AudioAnalyzer::setNoiseThresholdRatio(float value) {
    if (value >= 0.01f && value <= 1.0f) {
        noiseThresholdRatio = value;
        saveSetting("nThresh", noiseThresholdRatio);
        Serial.printf("[AudioAnalyzer] Set noiseThresholdRatio: %.2f\n", noiseThresholdRatio);
    } else {
        Serial.println("[AudioAnalyzer] Invalid noiseThresholdRatio value.");
    }
}

// Устанавливает коэффициент затухания для каждой полосы частот. Это значение контролирует, насколько быстро амплитуда сигнала в каждой полосе будет снижаться.
void AudioAnalyzer::setBandDecay(float value) {
    if (value >= 0.90f && value <= 1.0f) {
        bandDecay = value;
        saveSetting("bDecay", bandDecay);
        Serial.printf("[AudioAnalyzer] Set bandDecay: %.3f\n", bandDecay);
    } else {
        Serial.println("[AudioAnalyzer] Invalid bandDecay value.");
    }
}

// Устанавливает потолок для амплитуды каждой полосы. Это значение ограничивает максимальную амплитуду в каждой полосе частот.
void AudioAnalyzer::setBandCeiling(int value) {
    if (value >= 50 && value <= 1000) {
        bandCeiling = value;
        saveSetting("bCeil", bandCeiling);
        Serial.printf("[AudioAnalyzer] Set bandCeiling: %d\n", bandCeiling);
    } else {
        Serial.println("[AudioAnalyzer] Invalid bandCeiling value.");
    }
}

// --- Сглаживание полос ---
void AudioAnalyzer::smoothBands() {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        smoothedBands[i] = (smoothedBands[i] * (1.0f - alpha)) + (alpha * bands[i]);
    }
}

// --- Нормализация под высоту матрицы ---
void AudioAnalyzer::normalizeBands(uint16_t* heights, int matrixHeight) {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        if (maxAmplitude > 0) {
            heights[i] = map(smoothedBands[i], 0, maxAmplitude, 0, matrixHeight);
        } else {
            heights[i] = 0;
        }
        heights[i] = constrain(heights[i], 0, matrixHeight);
    }
}

// --- Получение нормализованных значений ---
void AudioAnalyzer::getNormalizedHeights(uint16_t* heights, int matrixHeight) {
    smoothBands();
    normalizeBands(heights, matrixHeight);
}

// --- Основной анализ звука ---
void AudioAnalyzer::processAudio(int micPin) {
    double avg = 0;
    double lastSample = analogRead(micPin);

    for (int i = 0; i < SAMPLES; i++) {
        double raw = analogRead(micPin);
        vReal[i] = alpha * raw + (1.0f - alpha) * lastSample;
        lastSample = vReal[i];
        avg += vReal[i];
        vImag[i] = 0;
    }

    avg /= SAMPLES;

    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] -= avg;
    }

    FFT.windowing(FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();
    calculateBands();
}

// --- Вычисление полос частот ---
void AudioAnalyzer::calculateBands() {
    const double freqPerBin = (double)SAMPLING_FREQUENCY / (double)SAMPLES;
    const double fMin = this->fMin;
    const double fMax = this->fMax;

    double totalRMS = 0.0;
    int totalBins = 0;

   
