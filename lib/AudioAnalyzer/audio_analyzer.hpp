#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <Arduino.h>
#include <arduinoFFT.h>
#include <Preferences.h>
#include "config.hpp"  // Определяет SAMPLES, MATRIX_WIDTH, SAMPLING_FREQUENCY

class AudioAnalyzer {
private:
    // Буферы для FFT
    double vReal[SAMPLES];
    double vImag[SAMPLES];

    // Значения частотных полос
    uint16_t bands[MATRIX_WIDTH];
    uint16_t smoothedBands[MATRIX_WIDTH];

    // Объекты FFT и NVS
    ArduinoFFT<double> FFT;
    Preferences preferences;

    // Максимальная амплитуда, для нормализации
    double maxAmplitude;

    // Порог шумоподавления (динамический)
    double dynamicNoiseThreshold;

    // Коэффициенты, подлежащие настройке
    float sensitivityReduction;     // Ослабление общей чувствительности
    float lowFreqGain;              // Усиление низких частот
    float midFreqGain;              // Усиление средних частот
    float highFreqGain;             // Усиление высоких частот
    float alpha;                    // Коэффициент сглаживания сигнала
    float fMin;                     // Минимальная частота анализируемого диапазона
    float fMax;                     // Максимальная частота анализируемого диапазона
    float noiseThresholdRatio;      // Относительный порог шума
    float bandDecay;                // Затухание полос
    int bandCeiling;                // Верхний предел амплитуды полос

    // Внутренние методы
    void calculateBands();
    void smoothBands();
    void normalizeBands(uint16_t* heights, int matrixHeight);
    void loadSettings();
    void saveSetting(const char* key, float value);

public:
    AudioAnalyzer();
    ~AudioAnalyzer();

    // Основной метод обработки аудиосигнала
    void processAudio(int micPin);

    // Получение нормализованных значений
    void getNormalizedHeights(uint16_t* heights, int matrixHeight);

    // Настройка параметров с проверками и сохранением
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

    // Инициализация и сброс
    void begin();
    void resetSettings();
};

#endif // AUDIO_ANALYZER_H
