#pragma once
#include <Preferences.h>
#include <arduinoFFT.h>
#include <cfloat>
#include "config.hpp"
#include "../LedMatrix/led_matrix.hpp"
#include "../ConfigurableBase/configurable_base.hpp"
#include "../../include/PersistantSetting.hpp"

// Количество сэмплов для анализа аудиосигнала (размер окна FFT)
#define SAMPLES 128

// Частота дискретизации аудиосигнала (Гц)
#define SAMPLING_FREQUENCY 8000

// --- Параметры по умолчанию для настроек аудиомодуля ---

// Чувствительность (порог подавления слабых сигналов), чем больше — тем меньше чувствительность
constexpr float DEFAULT_SENSITIVITY_REDUCTION = 5.0f;

// Усиление низких частот (басов)
constexpr float DEFAULT_LOW_FREQ_GAIN = 1.0f;

// Усиление средних частот
constexpr float DEFAULT_MID_FREQ_GAIN = 1.0f;

// Усиление высоких частот
constexpr float DEFAULT_HIGH_FREQ_GAIN = 1.0f;

// Коэффициент сглаживания (0 — нет сглаживания, 1 — максимальное сглаживание)
constexpr float DEFAULT_ALPHA = 0.5f;

// Минимальная частота анализируемого диапазона (Гц)
constexpr float DEFAULT_FMIN = 50.0f;

// Максимальная частота анализируемого диапазона (Гц)
constexpr float DEFAULT_FMAX = 10000.0f;

// Относительный порог шума (0.0...1.0), всё что ниже — считается шумом и игнорируется
constexpr float DEFAULT_NOISE_THRESHOLD_RATIO = 0.25f;

// Коэффициент затухания столбцов спектра (0 — мгновенное падение, 1 — нет затухания)
constexpr float DEFAULT_BAND_DECAY = 0.8f;

// Верхний предел амплитуды (используется для нормализации)
constexpr int   DEFAULT_BAND_CEILING = 1000;

// --- Описание параметров класса AudioAnalyzer ---

class AudioAnalyzer : public ConfigurableBase {
private:
    ArduinoFFT<double> FFT;  // Класс для быстрого преобразования Фурье (анализ спектра)
    double vReal[SAMPLES];   // Массив для хранения реальной части аудиосигнала
    double vImag[SAMPLES];   // Массив для хранения мнимой части (обычно нули)

    // --- Настраиваемые параметры (рабочие переменные) ---

    float sensitivityReduction; // Чувствительность (см. выше)
    float lowFreqGain;          // Усиление низких частот
    float midFreqGain;          // Усиление средних частот
    float highFreqGain;         // Усиление высоких частот
    float alpha;                // Коэффициент сглаживания
    float fMin;                 // Минимальная частота анализа
    float fMax;                 // Максимальная частота анализа
    float noiseThresholdRatio;  // Порог шума
    float bandDecay;            // Затухание столбцов спектра
    int bandCeiling;            // Верхний предел амплитуды

    // --- Внутренние массивы для визуализации спектра ---

    uint16_t bands[MATRIX_WIDTH];         // Текущие значения полос спектра
    uint16_t smoothedBands[MATRIX_WIDTH]; // Сглаженные значения полос

    float maxAmplitude;        // Максимальная амплитуда сигнала (для нормализации)
    float logPowerSmoothed;    // Сглаженное логарифмическое значение мощности сигнала

    // --- Статистика сигнала ---

    float minLogPower;         // Минимальная зафиксированная мощность (логарифмическая)
    float maxLogPower;         // Максимальная зафиксированная мощность (логарифмическая)
    int sampleCount;           // Количество обработанных сэмплов

    // --- Настройки для интеграции с ConfigurableBase ---
    // Каждая PersistantSetting связывает переменную с системой хранения/загрузки/сброса настроек

    PersistantSetting<float> sensitivityReductionSetting;   // Настройка чувствительности
    PersistantSetting<float> lowFreqGainSetting;            // Настройка усиления НЧ
    PersistantSetting<float> midFreqGainSetting;            // Настройка усиления СЧ
    PersistantSetting<float> highFreqGainSetting;           // Настройка усиления ВЧ
    PersistantSetting<float> alphaSetting;                  // Настройка сглаживания
    PersistantSetting<float> fMinSetting;                   // Настройка минимальной частоты
    PersistantSetting<float> fMaxSetting;                   // Настройка максимальной частоты
    PersistantSetting<float> noiseThresholdRatioSetting;    // Настройка порога шума
    PersistantSetting<float> bandDecaySetting;              // Настройка затухания
    PersistantSetting<int> bandCeilingSetting;              // Настройка потолка амплитуды

    // --- Внутренние методы обработки аудиосигнала ---
    void calculateBands(); // Расчёт полос спектра
    void smoothBands();    // Сглаживание значений полос
    void normalizeBands(uint16_t* heights, int matrixHeight); // Нормализация для вывода на матрицу
    void updateSignalStats(float currentLogPower); // Обновление статистики сигнала

public:
    AudioAnalyzer();
    ~AudioAnalyzer();

    void begin(); // Инициализация и загрузка настроек
    void processAudio(); // Основная обработка аудиосигнала
    void getNormalizedHeights(uint16_t* heights, int matrixHeight); // Получение данных для визуализации

    // Методы для получения статистики сигнала
    float getMinLogPower() const { return minLogPower; }
    float getMaxLogPower() const { return maxLogPower; }
    float getTotalLogRmsEnergy();
};
