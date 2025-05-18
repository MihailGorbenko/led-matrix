#include "audio_analyzer.hpp"
#include <nvs_flash.h>
#include <cmath>
#include <Arduino.h>

AudioAnalyzer::AudioAnalyzer()
    : ConfigurableBase("AudioAnalyzer", "Аудиоанализатор"),
      FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY),
      sensitivityReductionSetting("sensitivityReduction", "Снижение чувствительности", &sensitivityReduction, "audio", DEFAULT_SENSITIVITY_REDUCTION, 0.0f, 20.0f, 0.1f),
      lowFreqGainSetting("lowFreqGain", "НЧ усиление", &lowFreqGain, "audio", DEFAULT_LOW_FREQ_GAIN, 0.0f, 10.0f, 0.01f),
      midFreqGainSetting("midFreqGain", "СЧ усиление", &midFreqGain, "audio", DEFAULT_MID_FREQ_GAIN, 0.0f, 10.0f, 0.01f),
      highFreqGainSetting("highFreqGain", "ВЧ усиление", &highFreqGain, "audio", DEFAULT_HIGH_FREQ_GAIN, 0.0f, 10.0f, 0.01f),
      alphaSetting("alpha", "Сглаживание", &alpha, "audio", DEFAULT_ALPHA, 0.0f, 1.0f, 0.01f),
      fMinSetting("fMin", "Мин. частота", &fMin, "audio", DEFAULT_FMIN, 20.0f, 1000.0f, 1.0f),
      fMaxSetting("fMax", "Макс. частота", &fMax, "audio", DEFAULT_FMAX, 1000.0f, 20000.0f, 1.0f),
      noiseThresholdRatioSetting("noiseThresholdRatio", "Порог шума", &noiseThresholdRatio, "audio", DEFAULT_NOISE_THRESHOLD_RATIO, 0.0f, 1.0f, 0.01f),
      bandDecaySetting("bandDecay", "Затухание", &bandDecay, "audio", DEFAULT_BAND_DECAY, 0.0f, 1.0f, 0.01f),
      bandCeilingSetting("bandCeiling", "Потолок", &bandCeiling, "audio", DEFAULT_BAND_CEILING, 100, 10000, 1)
{
    registerSetting(&sensitivityReductionSetting);
    registerSetting(&lowFreqGainSetting);
    registerSetting(&midFreqGainSetting);
    registerSetting(&highFreqGainSetting);
    registerSetting(&alphaSetting);
    registerSetting(&fMinSetting);
    registerSetting(&fMaxSetting);
    registerSetting(&noiseThresholdRatioSetting);
    registerSetting(&bandDecaySetting);
    registerSetting(&bandCeilingSetting);

    memset(bands, 0, sizeof(bands));
    memset(smoothedBands, 0, sizeof(smoothedBands));
    memset(vReal, 0, sizeof(vReal));
    memset(vImag, 0, sizeof(vImag));
}

AudioAnalyzer::~AudioAnalyzer() {}

void AudioAnalyzer::begin() {
    Serial.println("[AudioAnalyzer] Инициализация...");
    loadConfig(); // Загружаем настройки из NVS
    Serial.println("[AudioAnalyzer] Настройки загружены из NVS.");
}

void AudioAnalyzer::updateSignalStats(float currentLogPower) {
    // Обновляем минимальное значение
    minLogPower = fminf(minLogPower, currentLogPower);

    // Добавляем затухание для максимального значения
    const float decayFactor = 0.90f; // Коэффициент затухания (0.0 - 1.0)
    maxLogPower = fmaxf(maxLogPower * decayFactor, currentLogPower);

    // Увеличиваем счётчик выборок
    sampleCount++;
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

    // Обновляем статистику сигнала
    updateSignalStats(logEnergy);

    return logEnergy;
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
        double fromFreq = DEFAULT_FMIN * pow(DEFAULT_FMAX / DEFAULT_FMIN, (double)b / MATRIX_WIDTH);
        double toFreq = DEFAULT_FMIN * pow(DEFAULT_FMAX / DEFAULT_FMIN, (double)(b + 1) / MATRIX_WIDTH);

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