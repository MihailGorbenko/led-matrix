#pragma once
#include <Preferences.h>
#include <arduinoFFT.h> // Ensure the arduinoFFT library is installed
#include <cfloat>
#include "config.hpp" // Подключаем файл конфигурации

// --- Дефолтные значения настроек ---
constexpr float DEFAULT_SENSITIVITY_REDUCTION = 5.0f;
constexpr float DEFAULT_LOW_FREQ_GAIN = 1.0f;
constexpr float DEFAULT_MID_FREQ_GAIN = 1.0f;
constexpr float DEFAULT_HIGH_FREQ_GAIN = 1.0f;
constexpr float DEFAULT_ALPHA = 0.5f;
constexpr float DEFAULT_FMIN = 50.0f;
constexpr float DEFAULT_FMAX = 10000.0f;
constexpr float DEFAULT_NOISE_THRESHOLD_RATIO = 0.25f;
constexpr float DEFAULT_BAND_DECAY = 0.8f; // Увеличьте значение для более медленного затухания
constexpr int   DEFAULT_BAND_CEILING = 1000;

class AudioAnalyzer {
private:
    Preferences preferences;
    ArduinoFFT<double> FFT; // Объект FFT
    double vReal[SAMPLES]; // Реальная часть FFT
    double vImag[SAMPLES]; // Мнимая часть FFT

    float sensitivityReduction;
    float lowFreqGain, midFreqGain, highFreqGain;
    float alpha;
    float fMin, fMax;
    float noiseThresholdRatio;
    float bandDecay;
    int bandCeiling;
    uint16_t bands[MATRIX_WIDTH];
    uint16_t smoothedBands[MATRIX_WIDTH];
    float maxAmplitude;
    float logPowerSmoothed;

    // Переменные для статистики сигнала
    float minLogPower;
    float maxLogPower;
    int sampleCount;

    void calculateBands();
    void smoothBands();
    void normalizeBands(uint16_t* heights, int matrixHeight);
    void updateSignalStats(float currentLogPower);

public:
    AudioAnalyzer();
    ~AudioAnalyzer();

    void begin();
    void processAudio();
    void getNormalizedHeights(uint16_t* heights, int matrixHeight);


    // Методы для настройки параметров
    void setSensitivityReduction(float value);
    void setLowFreqGain(float value);
    void setMidFreqGain(float value);
    void setHighFreqGain(float value);
    void setAlpha(float value);
    void setFMin(float value);
    void setFMax(float value);
    void setNoiseThresholdRatio(float value);
    void setBandDecay(float value);
    void setBandCeiling(int value);

    void loadSettings();
    void resetSettings();
    void saveSetting(const char* key, float value);
    void saveSetting(const char* key, int value);

    // Методы для получения статистики
    float getMinLogPower() const { return minLogPower; }
    float getMaxLogPower() const { return maxLogPower; }
    float getTotalLogRmsEnergy();
};
