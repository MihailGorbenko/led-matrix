#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <arduinoFFT.h>
#include "config.hpp" // Подключаем файл конфигурации

class AudioAnalyzer {
private:
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    uint16_t bands[MATRIX_WIDTH];
    uint16_t smoothedBands[MATRIX_WIDTH];
    ArduinoFFT<double> FFT;
    double maxAmplitude;
    double sensitivityReduction;
    double lowFreqGain;
    double midFreqGain;
    double highFreqGain;

public:
    AudioAnalyzer();
    void processAudio(int micPin);
    void calculateBands();
    void smoothBands();
    void normalizeBands(uint16_t* heights, int matrixHeight);
    void setSensitivityReduction(double reduction);
    void setLowFreqGain(double gain);
    void setMidFreqGain(double gain);
    void setHighFreqGain(double gain);
    uint16_t* getBands();
    uint16_t* getSmoothedBands();
    void getNormalizedHeights(uint16_t* heights, int matrixHeight);
};

#endif // AUDIO_ANALYZER_H