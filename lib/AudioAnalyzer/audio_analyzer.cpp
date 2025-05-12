#include "audio_analyzer.hpp"
#include <nvs_flash.h>
#include <cmath>
#include <Arduino.h>

AudioAnalyzer::AudioAnalyzer()
    : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY) { // Инициализация FFT
    // Инициализация массивов частотных полос
    memset(bands, 0, sizeof(bands));
    memset(smoothedBands, 0, sizeof(smoothedBands));

    // Инициализация настроек по умолчанию
    sensitivityReduction = DEFAULT_SENSITIVITY_REDUCTION;
    lowFreqGain = DEFAULT_LOW_FREQ_GAIN;
    midFreqGain = DEFAULT_MID_FREQ_GAIN;
    highFreqGain = DEFAULT_HIGH_FREQ_GAIN;
    alpha = DEFAULT_ALPHA;
    fMin = DEFAULT_FMIN;
    fMax = DEFAULT_FMAX;
    noiseThresholdRatio = DEFAULT_NOISE_THRESHOLD_RATIO;
    bandDecay = DEFAULT_BAND_DECAY;
    bandCeiling = DEFAULT_BAND_CEILING;
}

AudioAnalyzer::~AudioAnalyzer() {
    preferences.end();
}

void AudioAnalyzer::begin() {
    Serial.println("[AudioAnalyzer] Initializing...");
    if (!preferences.begin("audioanalyzer", false)) {
        Serial.println("[AudioAnalyzer] Failed to open preferences.");
        return;
    }
    loadSettings();
    Serial.println("[AudioAnalyzer] Initialization complete.");
}

void AudioAnalyzer::loadSettings() {
    if (!preferences.isKey("sensReduct")) {
        Serial.println("[AudioAnalyzer] Key 'sensReduct' not found. Using default value.");
        sensitivityReduction = DEFAULT_SENSITIVITY_REDUCTION;
        preferences.putFloat("sensReduct", sensitivityReduction);
    } else {
        sensitivityReduction = preferences.getFloat("sensReduct", DEFAULT_SENSITIVITY_REDUCTION);
    }
    Serial.printf("[AudioAnalyzer] Loaded sensitivityReduction: %.2f\n", sensitivityReduction);

    if (!preferences.isKey("lowGain")) {
        Serial.println("[AudioAnalyzer] Key 'lowGain' not found. Using default value.");
        lowFreqGain = DEFAULT_LOW_FREQ_GAIN;
        preferences.putFloat("lowGain", lowFreqGain);
    } else {
        lowFreqGain = preferences.getFloat("lowGain", DEFAULT_LOW_FREQ_GAIN);
    }
    Serial.printf("[AudioAnalyzer] Loaded lowFreqGain: %.2f\n", lowFreqGain);

    if (!preferences.isKey("midGain")) {
        Serial.println("[AudioAnalyzer] Key 'midGain' not found. Using default value.");
        midFreqGain = DEFAULT_MID_FREQ_GAIN;
        preferences.putFloat("midGain", midFreqGain);
    } else {
        midFreqGain = preferences.getFloat("midGain", DEFAULT_MID_FREQ_GAIN);
    }
    Serial.printf("[AudioAnalyzer] Loaded midFreqGain: %.2f\n", midFreqGain);

    if (!preferences.isKey("highGain")) {
        Serial.println("[AudioAnalyzer] Key 'highGain' not found. Using default value.");
        highFreqGain = DEFAULT_HIGH_FREQ_GAIN;
        preferences.putFloat("highGain", highFreqGain);
    } else {
        highFreqGain = preferences.getFloat("highGain", DEFAULT_HIGH_FREQ_GAIN);
    }
    Serial.printf("[AudioAnalyzer] Loaded highFreqGain: %.2f\n", highFreqGain);

    if (!preferences.isKey("alpha")) {
        Serial.println("[AudioAnalyzer] Key 'alpha' not found. Using default value.");
        alpha = DEFAULT_ALPHA;
        preferences.putFloat("alpha", alpha);
    } else {
        alpha = preferences.getFloat("alpha", DEFAULT_ALPHA);
    }
    Serial.printf("[AudioAnalyzer] Loaded alpha: %.2f\n", alpha);

    if (!preferences.isKey("fMin")) {
        Serial.println("[AudioAnalyzer] Key 'fMin' not found. Using default value.");
        fMin = DEFAULT_FMIN;
        preferences.putFloat("fMin", fMin);
    } else {
        fMin = preferences.getFloat("fMin", DEFAULT_FMIN);
    }
    Serial.printf("[AudioAnalyzer] Loaded fMin: %.2f\n", fMin);

    if (!preferences.isKey("fMax")) {
        Serial.println("[AudioAnalyzer] Key 'fMax' not found. Using default value.");
        fMax = DEFAULT_FMAX;
        preferences.putFloat("fMax", fMax);
    } else {
        fMax = preferences.getFloat("fMax", DEFAULT_FMAX);
    }
    Serial.printf("[AudioAnalyzer] Loaded fMax: %.2f\n", fMax);

    if (!preferences.isKey("nThresh")) {
        Serial.println("[AudioAnalyzer] Key 'nThresh' not found. Using default value.");
        noiseThresholdRatio = DEFAULT_NOISE_THRESHOLD_RATIO;
        preferences.putFloat("nThresh", noiseThresholdRatio);
    } else {
        noiseThresholdRatio = preferences.getFloat("nThresh", DEFAULT_NOISE_THRESHOLD_RATIO);
    }
    Serial.printf("[AudioAnalyzer] Loaded noiseThresholdRatio: %.2f\n", noiseThresholdRatio);

    if (!preferences.isKey("bDecay")) {
        Serial.println("[AudioAnalyzer] Key 'bDecay' not found. Using default value.");
        bandDecay = DEFAULT_BAND_DECAY;
        preferences.putFloat("bDecay", bandDecay);
    } else {
        bandDecay = preferences.getFloat("bDecay", DEFAULT_BAND_DECAY);
    }
    Serial.printf("[AudioAnalyzer] Loaded bandDecay: %.2f\n", bandDecay);

    if (!preferences.isKey("bCeil")) {
        Serial.println("[AudioAnalyzer] Key 'bCeil' not found. Using default value.");
        bandCeiling = DEFAULT_BAND_CEILING;
        preferences.putInt("bCeil", bandCeiling);
    } else {
        bandCeiling = preferences.getInt("bCeil", DEFAULT_BAND_CEILING);
    }
    Serial.printf("[AudioAnalyzer] Loaded bandCeiling: %d\n", bandCeiling);
    preferences.end();
}

float AudioAnalyzer::getTotalLogRmsEnergy() {
    float rmsSum = 0.0f;

    for (int i = 0; i < SAMPLES / 2; i++) {
        rmsSum += vReal[i] * vReal[i];
    }

    float rms = sqrtf(rmsSum / (SAMPLES / 2));

    // Вычисляем логарифмическую энергию, добавляя 1.0 для защиты от log(0)
    float logEnergy = 10.0f * log10f(rms + 1.0f);

    // Ограничиваем значение
    logEnergy = constrain(logEnergy, 0.0f, (float)bandCeiling);

    return logEnergy;
}

void AudioAnalyzer::resetSettings() {
    if (!preferences.begin("audioanalyzer", false)) {
        Serial.println("[AudioAnalyzer] Failed to open preferences for resetting.");
        return;
    }
    preferences.clear();
    preferences.end();
    begin();
}

void AudioAnalyzer::saveSetting(const char* key, float value) {
    if (!preferences.begin("audioanalyzer", false)) {
        Serial.println("[AudioAnalyzer] Failed to open preferences for saving.");
        return;
    }
    preferences.putFloat(key, value);
    Serial.printf("[AudioAnalyzer] Saved %s: %.2f\n", key, value);
    preferences.end();
}

void AudioAnalyzer::saveSetting(const char* key, int value) {
    if (!preferences.begin("audioanalyzer", false)) {
        Serial.println("[AudioAnalyzer] Failed to open preferences for saving.");
        return;
    }
    preferences.putInt(key, value);
    Serial.printf("[AudioAnalyzer] Saved %s: %d\n", key, value);
    preferences.end();
}

void AudioAnalyzer::setSensitivityReduction(float value) {
    if (value >= 0.1f && value <= 100.0f) {
        sensitivityReduction = value;
        saveSetting("sensReduct", value);
    }
}

void AudioAnalyzer::setLowFreqGain(float value) {
    if (value >= 0.0f && value <= 10.0f) {
        lowFreqGain = value;
        saveSetting("lowGain", value);
    }
}

void AudioAnalyzer::setMidFreqGain(float value) {
    if (value >= 0.0f && value <= 10.0f) {
        midFreqGain = value;
        saveSetting("midGain", value);
    }
}

void AudioAnalyzer::setHighFreqGain(float value) {
    if (value >= 0.0f && value <= 10.0f) {
        highFreqGain = value;
        saveSetting("highGain", value);
    }
}

void AudioAnalyzer::setAlpha(float value) {
    if (value >= 0.01f && value <= 1.0f) {
        alpha = value;
        saveSetting("alpha", value);
    }
}

void AudioAnalyzer::setFMin(float value) {
    if (value >= 10.0f && value <= 1000.0f) {
        fMin = value;
        saveSetting("fMin", value);
    }
}

void AudioAnalyzer::setFMax(float value) {
    if (value >= 1000.0f && value <= 30000.0f) {
        fMax = value;
        saveSetting("fMax", value);
    }
}

void AudioAnalyzer::setNoiseThresholdRatio(float value) {
    if (value >= 0.01f && value <= 1.0f) {
        noiseThresholdRatio = value;
        saveSetting("nThresh", value);
    }
}

void AudioAnalyzer::setBandDecay(float value) {
    if (value >= 0.90f && value <= 1.0f) {
        bandDecay = value;
        saveSetting("bDecay", value);
    }
}

void AudioAnalyzer::setBandCeiling(int value) {
    if (value >= 50 && value <= 1000) {
        bandCeiling = value;
        saveSetting("bCeil", value);
    }
}

void AudioAnalyzer::processAudio() {
    double avg = 0;
    double lastSample = analogRead(MIC_PIN);

    for (int i = 0; i < SAMPLES; i++) {
        double raw = analogRead(MIC_PIN);
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

void AudioAnalyzer::calculateBands() {
    if (fMin <= 0 || fMax <= 0) {
        Serial.println("[AudioAnalyzer] Invalid frequency range.");
        return;
    }

    const double freqPerBin = (double)SAMPLING_FREQUENCY / SAMPLES;
    const int totalBins = SAMPLES / 2;

    double rmsSum = 0;
    for (int i = 0; i < totalBins; i++) {
        rmsSum += vReal[i] * vReal[i];
    }
    float rms = sqrt(rmsSum / totalBins);
    float threshold = rms * noiseThresholdRatio;

    maxAmplitude = 0;

    for (int b = 0; b < MATRIX_WIDTH; b++) {
        double fromFreq = fMin * pow(fMax / fMin, (double)b / MATRIX_WIDTH);
        double toFreq = fMin * pow(fMax / fMin, (double)(b + 1) / MATRIX_WIDTH);

        int fromBin = (int)(fromFreq / freqPerBin);
        int toBin = (int)(toFreq / freqPerBin);

        fromBin = constrain(fromBin, 0, totalBins - 1);
        toBin = constrain(toBin, fromBin + 1, totalBins);

        double sum = 0;
        for (int i = fromBin; i < toBin; i++) {
            float amplitude = vReal[i]; // Используем vReal[i] вместо FFT.read(i)
            if (amplitude > threshold) {
                sum += amplitude;
            }
        }

        sum /= sensitivityReduction;

        if (b < MATRIX_WIDTH / 3) sum *= lowFreqGain;
        else if (b < 2 * MATRIX_WIDTH / 3) sum *= midFreqGain;
        else sum *= highFreqGain;

        bands[b] *= bandDecay;
        if (sum > bands[b]) bands[b] = sum;

        if (bands[b] > maxAmplitude) maxAmplitude = bands[b];
    }

    maxAmplitude = std::min(maxAmplitude, (float)bandCeiling);
}

void AudioAnalyzer::smoothBands() {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        smoothedBands[i] = (1.0f - alpha) * smoothedBands[i] + alpha * bands[i];
    }
}

void AudioAnalyzer::normalizeBands(uint16_t* heights, int matrixHeight) {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        heights[i] = (maxAmplitude > 0)
            ? map(smoothedBands[i], 0, maxAmplitude, 0, matrixHeight)
            : 0;
        heights[i] = constrain(heights[i], 0, matrixHeight);
    }
}

void AudioAnalyzer::getNormalizedHeights(uint16_t* heights, int matrixHeight) {
    smoothBands();
    normalizeBands(heights, matrixHeight);
}
