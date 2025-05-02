#pragma once
#include <Preferences.h>
#include <arduinoFFT.h>

#define SAMPLES 256
#define SAMPLING_FREQUENCY 40000
#define MATRIX_WIDTH 10

// --- Дефолтные значения настроек ---
constexpr float DEFAULT_SENSITIVITY_REDUCTION = 5.0f;
constexpr float DEFAULT_LOW_FREQ_GAIN = 1.0f;
constexpr float DEFAULT_MID_FREQ_GAIN = 1.0f;
constexpr float DEFAULT_HIGH_FREQ_GAIN = 1.0f;
constexpr float DEFAULT_ALPHA = 0.2f;
constexpr float DEFAULT_FMIN = 50.0f;
constexpr float DEFAULT_FMAX = 10000.0f;
constexpr float DEFAULT_NOISE_THRESHOLD_RATIO = 0.25f;
constexpr float DEFAULT_BAND_DECAY = 0.95f;
constexpr int   DEFAULT_BAND_CEILING = 1000

class AudioAnalyzer {
public:
    AudioAnalyzer();
    ~AudioAnalyzer();

    void begin();
    void processAudio(int micPin);
    void getNormalizedHeights(uint16_t* heights, int matrixHeight);

    // Установка коэффициентов
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

    void resetSettings(); // --- Сброс настроек по умолчанию ---

private:
    Preferences preferences;
    arduinoFFT FFT;
    double vReal[SAMPLES];
    double vImag[SAMPLES];

    float bands[MATRIX_WIDTH];
    float smoothedBands[MATRIX_WIDTH];
    float maxAmplitude = 1.0f;

    // Настройки
    float sensitivityReduction;
    float lowFreqGain;
    float midFreqGain;
    float highFreqGain;
    float alpha;
    float fMin;
    float fMax;
    float noiseThresholdRatio;
    float bandDecay;
    int bandCeiling;

    void loadSettings();
    void saveSetting(const char* key, float value);
    void saveSetting(const char* key, int value);
    void smoothBands();
    void normalizeBands(uint16_t* heights, int matrixHeight);
    void calculateBands();
};
