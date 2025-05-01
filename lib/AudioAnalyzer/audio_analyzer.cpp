#include "audio_analyzer.hpp"
#include <nvs_flash.h>

// Конструктор
AudioAnalyzer::AudioAnalyzer()
    : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY), // Инициализация FFT
      maxAmplitude(1), sensitivityReduction(1.0),
      lowFreqGain(1.0), midFreqGain(1.0), highFreqGain(1.0),
      dynamicNoiseThreshold(0.0) {
    memset(bands, 0, sizeof(bands));
    memset(smoothedBands, 0, sizeof(smoothedBands));
    Serial.println("[AudioAnalyzer] Constructor called.");
}

// Деструктор
AudioAnalyzer::~AudioAnalyzer() {
    preferences.end(); // Закрываем пространство имен
    Serial.println("[AudioAnalyzer] Preferences closed.");
}

// Инициализация
void AudioAnalyzer::begin() {
    Serial.println("[AudioAnalyzer] Initializing...");
    preferences.begin("audioanalyzer", true); // Открываем пространство имен
    loadSettings(); // Загрузка сохранённых настроек
    preferences.end(); // Закрываем пространство имен
    Serial.println("[AudioAnalyzer] Initialization complete.");
}

// Загрузка настроек из памяти
void AudioAnalyzer::loadSettings() {
    sensitivityReduction = preferences.getFloat("sensReduct", 5.0);
    Serial.printf("[AudioAnalyzer] Loaded sensitivityReduction: %.2f\n", sensitivityReduction);

    lowFreqGain = preferences.getFloat("lowGain", 1.0);
    Serial.printf("[AudioAnalyzer] Loaded lowFreqGain: %.2f\n", lowFreqGain);

    midFreqGain = preferences.getFloat("midGain", 1.0);
    Serial.printf("[AudioAnalyzer] Loaded midFreqGain: %.2f\n", midFreqGain);

    highFreqGain = preferences.getFloat("highGain", 1.0);
    Serial.printf("[AudioAnalyzer] Loaded highFreqGain: %.2f\n", highFreqGain);
}

// Сохранение настройки в память
void AudioAnalyzer::saveSetting(const char* key, float value) {
    preferences.begin("audioanalyzer",false); // Открываем пространство имен
    preferences.putFloat(key, value);
    preferences.end(); // Закрываем пространство имен
    Serial.printf("[AudioAnalyzer] Saved %s: %.2f\n", key, value);
}

// Установка общей чувствительности
void AudioAnalyzer::setSensitivityReduction(double reduction) {
    sensitivityReduction = reduction > 0 ? reduction : 1.0;
    saveSetting("sensReduct", sensitivityReduction);
    Serial.printf("[AudioAnalyzer] Set sensitivityReduction: %.2f\n", sensitivityReduction);
}

// Установка усиления для низких частот
void AudioAnalyzer::setLowFreqGain(double gain) {
    lowFreqGain = gain;
    saveSetting("lowGain", lowFreqGain);
    Serial.printf("[AudioAnalyzer] Set lowFreqGain: %.2f\n", lowFreqGain);
}

// Установка усиления для средних частот
void AudioAnalyzer::setMidFreqGain(double gain) {
    midFreqGain = gain;
    saveSetting("midGain", midFreqGain);
    Serial.printf("[AudioAnalyzer] Set midFreqGain: %.2f\n", midFreqGain);
}

// Установка усиления для высоких частот
void AudioAnalyzer::setHighFreqGain(double gain) {
    highFreqGain = gain;
    saveSetting("highGain", highFreqGain);
    Serial.printf("[AudioAnalyzer] Set highFreqGain: %.2f\n", highFreqGain);
}

// Сглаживание полос
void AudioAnalyzer::smoothBands() {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        smoothedBands[i] = (smoothedBands[i] * 3 + bands[i]) / 4;
    }

}

// Нормализация полос для высоты матрицы
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

// Получение нормализованных высот
void AudioAnalyzer::getNormalizedHeights(uint16_t* heights, int matrixHeight) {
    smoothBands();
    normalizeBands(heights, matrixHeight);
   
}

// Обработка аудиосигнала
void AudioAnalyzer::processAudio(int micPin) {
    double avg = 0;

    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = analogRead(micPin);
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

// Вычисление амплитуд в полосах
void AudioAnalyzer::calculateBands() {
    const double freqPerBin = (double)SAMPLING_FREQUENCY / (double)SAMPLES;
    const double fMin = 50.0;
    const double fMax = 20000.0;

    double totalRMS = 0.0;
    int totalBins = 0;

    for (int i = 0; i < SAMPLES / 2; i++) {
        totalRMS += vReal[i] * vReal[i];
        totalBins++;
    }

    totalRMS = sqrt(totalRMS / totalBins);
    dynamicNoiseThreshold = totalRMS * 0.1;

    for (int b = 0; b < MATRIX_WIDTH; b++) {
        double fromFreq = fMin * pow(fMax / fMin, (double)b / MATRIX_WIDTH);
        double toFreq = fMin * pow(fMax / fMin, (double)(b + 1) / MATRIX_WIDTH);

        int fromBin = (int)(fromFreq / freqPerBin);
        int toBin = (int)(toFreq / freqPerBin);

        fromBin = constrain(fromBin, 0, SAMPLES / 2 - 1);
        toBin = constrain(toBin, fromBin + 1, SAMPLES / 2);

        if (toBin <= fromBin) continue;

        double sumSquares = 0;
        for (int i = fromBin; i < toBin; i++) {
            sumSquares += vReal[i] * vReal[i];
        }

        double rms = sqrt(sumSquares / (toBin - fromBin));

        if (rms < dynamicNoiseThreshold) rms = 0;
        if (sensitivityReduction > 0) rms /= sensitivityReduction;

        if (b < MATRIX_WIDTH / 3) rms *= lowFreqGain;
        else if (b < 2 * MATRIX_WIDTH / 3) rms *= midFreqGain;
        else rms *= highFreqGain;

        rms = constrain(rms, 0, 400);
        bands[b] = (uint16_t)rms;

        if (bands[b] > maxAmplitude) {
            maxAmplitude = bands[b];
        } else {
            maxAmplitude = max(maxAmplitude * 0.995, 1.0);
        }
    }


}

