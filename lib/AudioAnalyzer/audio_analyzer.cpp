#include "audio_analyzer.hpp"
#include <nvs_flash.h>

// Конструктор
AudioAnalyzer::AudioAnalyzer()
    : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY), // Инициализация FFT с заданными параметрами
      maxAmplitude(1), // Начальное значение максимальной амплитуды
      sensitivityReduction(5.0), // Коэффициент уменьшения чувствительности
      lowFreqGain(1.0), midFreqGain(1.0), highFreqGain(1.0), // Усиление для низких, средних и высоких частот
      dynamicNoiseThreshold(0.0) { // Инициализация динамического порога
    memset(bands, 0, sizeof(bands)); // Инициализация массива полос нулями
    memset(smoothedBands, 0, sizeof(smoothedBands)); // Инициализация сглаженных полос нулями

    // Инициализация NVS
    Serial.println("[NVS] Initializing NVS...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        Serial.println("[NVS] NVS corrupted, erasing...");
        nvs_flash_erase();
        err = nvs_flash_init();
    }

    if (err == ESP_OK) {
        Serial.println("[NVS] NVS initialized successfully.");
    } else {
        Serial.printf("[NVS] Failed to initialize NVS. Error: %d\n", err);
        while (true); // Остановить выполнение, если NVS не инициализировался
    }

    preferences.begin("audioanalyzer", false); // Открываем пространство имен для чтения и записи
    loadSettings(); // Загрузка сохранённых настроек
}

// Деструктор
AudioAnalyzer::~AudioAnalyzer() {
    preferences.end(); // Закрываем пространство имен
    Serial.println("[NVS] Preferences closed.");
}

// Загрузка настроек из памяти
void AudioAnalyzer::loadSettings() {
    sensitivityReduction = preferences.getFloat("sensReduct", 5.0);
    Serial.printf("[NVS] Loaded sensReduct: %.2f\n", sensitivityReduction);

    lowFreqGain = preferences.getFloat("lowGain", 1.0);
    Serial.printf("[NVS] Loaded lowGain: %.2f\n", lowFreqGain);

    midFreqGain = preferences.getFloat("midGain", 1.0);
    Serial.printf("[NVS] Loaded midGain: %.2f\n", midFreqGain);

    highFreqGain = preferences.getFloat("highGain", 1.0);
    Serial.printf("[NVS] Loaded highGain: %.2f\n", highFreqGain);
}

// Сохранение настройки в память
void AudioAnalyzer::saveSetting(const char* key, float value) {
    preferences.putFloat(key, value);
    Serial.printf("[NVS] Saved %s: %.2f\n", key, value);
}

// Установка общей чувствительности
void AudioAnalyzer::setSensitivityReduction(double reduction) {
    sensitivityReduction = reduction > 0 ? reduction : 1.0;
    saveSetting("sensReduct", sensitivityReduction);
}

// Установка усиления для низких частот
void AudioAnalyzer::setLowFreqGain(double gain) {
    lowFreqGain = gain;
    saveSetting("lowGain", lowFreqGain);
}

// Установка усиления для средних частот
void AudioAnalyzer::setMidFreqGain(double gain) {
    midFreqGain = gain;
    saveSetting("midGain", midFreqGain);
}

// Установка усиления для высоких частот
void AudioAnalyzer::setHighFreqGain(double gain) {
    highFreqGain = gain;
    saveSetting("highGain", highFreqGain);
}

// Сглаживание полос
void AudioAnalyzer::smoothBands() {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        // Сглаживаем значения полос, используя взвешенное среднее
        smoothedBands[i] = (smoothedBands[i] * 3 + bands[i]) / 4;
    }
}

// Нормализация полос для высоты матрицы
void AudioAnalyzer::normalizeBands(uint16_t* heights, int matrixHeight) {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        if (maxAmplitude > 0) {
            // Нормализуем значения полос в диапазон от 0 до высоты матрицы
            heights[i] = map(smoothedBands[i], 0, maxAmplitude, 0, matrixHeight);
        } else {
            heights[i] = 0; // Если maxAmplitude равен 0, устанавливаем высоту в 0
        }
        // Ограничиваем значения в пределах высоты матрицы
        heights[i] = constrain(heights[i], 0, matrixHeight);
    }
}

// Получение нормализованных высот
void AudioAnalyzer::getNormalizedHeights(uint16_t* heights, int matrixHeight) {
    smoothBands(); // Сглаживаем полосы
    normalizeBands(heights, matrixHeight); // Нормализуем полосы
}

// Обработка аудиосигнала
void AudioAnalyzer::processAudio(int micPin) {
    double avg = 0;

    // Считываем значения с микрофона и вычисляем среднее
    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = analogRead(micPin); // Чтение значения с микрофона
        avg += vReal[i]; // Суммируем значения для вычисления среднего
        vImag[i] = 0; // Устанавливаем мнимую часть в 0
    }

    avg /= SAMPLES; // Вычисляем среднее значение

    // Убираем постоянную составляющую из сигнала
    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] -= avg;
    }

    // Применяем окно Хэмминга для уменьшения спектральных утечек
    FFT.windowing(FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);

    // Выполняем FFT
    FFT.compute(FFT_FORWARD);

    // Преобразуем комплексные числа в амплитуды
    FFT.complexToMagnitude();

    // Вычисляем амплитуды в полосах
    calculateBands();
}

// Вычисление амплитуд в полосах
void AudioAnalyzer::calculateBands() {
    const double freqPerBin = (double)SAMPLING_FREQUENCY / (double)SAMPLES; // Частота, соответствующая одному бин-индексу
    const double fMin = 50.0; // Минимальная частота
    const double fMax = 20000.0; // Максимальная частота

    // Вычисляем средний уровень сигнала для адаптации порога
    double totalRMS = 0.0;
    int totalBins = 0;

    for (int i = 0; i < SAMPLES / 2; i++) {
        totalRMS += vReal[i] * vReal[i]; // Суммируем квадраты амплитуд
        totalBins++;
    }

    totalRMS = sqrt(totalRMS / totalBins); // Вычисляем среднеквадратичное значение
    dynamicNoiseThreshold = totalRMS * 0.1; // Устанавливаем порог как 10% от RMS

    for (int b = 0; b < MATRIX_WIDTH; b++) {
        // Вычисляем частотные диапазоны для текущей полосы
        double fromFreq = fMin * pow(fMax / fMin, (double)b / MATRIX_WIDTH);
        double toFreq   = fMin * pow(fMax / fMin, (double)(b + 1) / MATRIX_WIDTH);

        int fromBin = (int)(fromFreq / freqPerBin); // Индекс начальной частоты
        int toBin   = (int)(toFreq / freqPerBin); // Индекс конечной частоты

        fromBin = constrain(fromBin, 0, SAMPLES / 2 - 1); // Ограничиваем индексы
        toBin   = constrain(toBin, fromBin + 1, SAMPLES / 2);

        if (toBin <= fromBin) continue; // Пропускаем некорректные диапазоны

        double sumSquares = 0;
        for (int i = fromBin; i < toBin; i++) {
            sumSquares += vReal[i] * vReal[i]; // Суммируем квадраты амплитуд
        }

        double rms = sqrt(sumSquares / (toBin - fromBin)); // Вычисляем RMS для полосы

        // Применяем динамический порог
        if (rms < dynamicNoiseThreshold) rms = 0;
        if (sensitivityReduction > 0) rms /= sensitivityReduction;

        // Применяем усиление в зависимости от частотного диапазона
        if (b < MATRIX_WIDTH / 3) rms *= lowFreqGain;
        else if (b < 2 * MATRIX_WIDTH / 3) rms *= midFreqGain;
        else rms *= highFreqGain;

        rms = constrain(rms, 0, 400); // Ограничиваем значение RMS
        bands[b] = (uint16_t)rms; // Сохраняем значение в массив полос

        // Обновляем максимальную амплитуду
        if (bands[b] > maxAmplitude) {
            maxAmplitude = bands[b];
        } else {
            maxAmplitude = max(maxAmplitude * 0.995, 1.0); // Постепенное уменьшение maxAmplitude
        }
    }
}

