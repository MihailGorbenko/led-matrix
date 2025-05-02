#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <arduinoFFT.h>
#include "config.hpp" // Подключаем файл конфигурации
#include <Arduino.h>
#include <Preferences.h>

// Константы для настройки AudioAnalyzer
constexpr double DEFAULT_FMIN = 50.0;                // Минимальная частота
constexpr double DEFAULT_FMAX = 20000.0;            // Максимальная частота
constexpr double DEFAULT_NOISE_THRESHOLD_RATIO = 0.25; // Коэффициент порога шума
constexpr double DEFAULT_SENSITIVITY_REDUCTION = 5.0; // Коэффициент чувствительности
constexpr double DEFAULT_LOW_FREQ_GAIN = 0.8;       // Усиление низких частот
constexpr double DEFAULT_MID_FREQ_GAIN = 1.1;       // Усиление средних частот
constexpr double DEFAULT_HIGH_FREQ_GAIN = 1.1;      // Усиление высоких частот
constexpr double DEFAULT_MAX_AMPLITUDE_DECAY = 0.995; // Затухание максимальной амплитуды
constexpr uint16_t DEFAULT_RMS_CONSTRAINT = 400;    // Ограничение RMS

class AudioAnalyzer {
private:
    void calculateBands();
    void smoothBands();
    void normalizeBands(uint16_t* heights, int matrixHeight);
    void loadSettings();
    void saveSetting(const char* key, float value);
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    uint16_t bands[MATRIX_WIDTH];
    uint16_t smoothedBands[MATRIX_WIDTH];
    ArduinoFFT<double> FFT;
    Preferences preferences;
    double maxAmplitude;
    double sensitivityReduction;
    double lowFreqGain;
    double midFreqGain;
    double highFreqGain;
    double dynamicNoiseThreshold;
    double noiseThresholdRatio; // Коэффициент порога шума

public:
    AudioAnalyzer();
    ~AudioAnalyzer();
    void processAudio(int micPin);
    void setSensitivityReduction(double reduction);
    void setLowFreqGain(double gain);
    void setMidFreqGain(double gain);
    void setHighFreqGain(double gain);
    void setNoiseThresholdRatio(double ratio); // Новый сеттер
    void getNormalizedHeights(uint16_t* heights, int matrixHeight);
    void resetSettings();
    void begin();
};

#endif // AUDIO_ANALYZER_H