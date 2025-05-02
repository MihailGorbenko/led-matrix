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
    if (!preferences.begin("audioanalyzer", true)) {
        Serial.println("[AudioAnalyzer] Failed to open preferences for reading.");
        return;
    }

    sensitivityReduction = preferences.getFloat("sensReduct", DEFAULT_SENSITIVITY_REDUCTION);
    Serial.printf("[AudioAnalyzer] Loaded sensitivityReduction: %.2f\n", sensitivityReduction);

    lowFreqGain = preferences.getFloat("lowGain", DEFAULT_LOW_FREQ_GAIN);
    Serial.printf("[AudioAnalyzer] Loaded lowFreqGain: %.2f\n", lowFreqGain);

    midFreqGain = preferences.getFloat("midGain", DEFAULT_MID_FREQ_GAIN);
    Serial.printf("[AudioAnalyzer] Loaded midFreqGain: %.2f\n", midFreqGain);

    highFreqGain = preferences.getFloat("highGain", DEFAULT_HIGH_FREQ_GAIN);
    Serial.printf("[AudioAnalyzer] Loaded highFreqGain: %.2f\n", highFreqGain);

    alpha = preferences.getFloat("alpha", DEFAULT_ALPHA);
    Serial.printf("[AudioAnalyzer] Loaded alpha: %.2f\n", alpha);

    fMin = preferences.getFloat("fMin", DEFAULT_FMIN);
    Serial.printf("[AudioAnalyzer] Loaded fMin: %.2f\n", fMin);

    fMax = preferences.getFloat("fMax", DEFAULT_FMAX);
    Serial.printf("[AudioAnalyzer] Loaded fMax: %.2f\n", fMax);

    noiseThresholdRatio = preferences.getFloat("nThresh", DEFAULT_NOISE_THRESHOLD_RATIO);
    Serial.printf("[AudioAnalyzer] Loaded noiseThresholdRatio: %.2f\n", noiseThresholdRatio);

    bandDecay = preferences.getFloat("bDecay", DEFAULT_BAND_DECAY);
    Serial.printf("[AudioAnalyzer] Loaded bandDecay: %.2f\n", bandDecay);

    bandCeiling = preferences.getInt("bCeil", DEFAULT_BAND_CEILING);
    Serial.printf("[AudioAnalyzer] Loaded bandCeiling: %d\n", bandCeiling);

    preferences.end();
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

    FFT.Windowing(FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
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
            float amplitude = FFT.read(i);
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
