#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <arduinoFFT.h>
#include "config.hpp" // Подключаем файл конфигурации
#include <Arduino.h>
#include <Preferences.h>
#include "async_serial.hpp"

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
    AsyncSerial& serial; // Добавляем член класса для хранения ссылки на AsyncSerial
    double maxAmplitude;
    double sensitivityReduction;
    double lowFreqGain;
    double midFreqGain;
    double highFreqGain;
    double dynamicNoiseThreshold;

public:
    AudioAnalyzer(AsyncSerial& asyncSerial);
    ~AudioAnalyzer();
    void processAudio(int micPin);
    void setSensitivityReduction(double reduction);
    void setLowFreqGain(double gain);
    void setMidFreqGain(double gain);
    void setHighFreqGain(double gain);
    void getNormalizedHeights(uint16_t* heights, int matrixHeight);
    void resetSettings();
};

#endif // AUDIO_ANALYZER_H