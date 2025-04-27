#include "audio_analyzer.hpp"

// Конструктор
AudioAnalyzer::AudioAnalyzer()
    : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY),
      maxAmplitude(1),
      sensitivityReduction(5.0),
      lowFreqGain(1.0), midFreqGain(1.0), highFreqGain(1.0) {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        smoothedBands[i] = 0;
    }
}

// Установка общей чувствительности
void AudioAnalyzer::setSensitivityReduction(double reduction) {
    sensitivityReduction = reduction > 0 ? reduction : 1.0; // Защита от некорректных значений
}

// Установка усиления для низких частот
void AudioAnalyzer::setLowFreqGain(double gain) {
    lowFreqGain = gain;
}

// Установка усиления для средних частот
void AudioAnalyzer::setMidFreqGain(double gain) {
    midFreqGain = gain;
}

// Установка усиления для высоких частот
void AudioAnalyzer::setHighFreqGain(double gain) {
    highFreqGain = gain;
}

// Получение массива полос
uint16_t* AudioAnalyzer::getBands() {
    return bands;
}

// Получение сглаженных полос
uint16_t* AudioAnalyzer::getSmoothedBands() {
    return smoothedBands;
}

// Сглаживание полос
void AudioAnalyzer::smoothBands() {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        smoothedBands[i] = (smoothedBands[i] * 3 + bands[i]) / 4; // Простое сглаживание
    }
}

// Нормализация полос для высоты матрицы
void AudioAnalyzer::normalizeBands(uint16_t* heights, int matrixHeight) {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        if (maxAmplitude > 0) { // Защита от деления на ноль
            heights[i] = map(smoothedBands[i], 0, maxAmplitude, 0, matrixHeight);
        } else {
            heights[i] = 0;
        }
        heights[i] = constrain(heights[i], 0, matrixHeight);
    }
}

// Получение нормализованных высот
void AudioAnalyzer::getNormalizedHeights(uint16_t* heights, int matrixHeight) {
    // Сглаживаем полосы
    smoothBands();

    // Нормализуем полосы
    normalizeBands(heights, matrixHeight);
}

// Обработка аудиосигнала
void AudioAnalyzer::processAudio(int micPin) {
    double avg = 0;

    // Считываем значения с микрофона и вычисляем среднее
    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = analogRead(micPin);
        avg += vReal[i];
        vImag[i] = 0;
    }

    avg /= SAMPLES;

    // Убираем среднее значение из массива
    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] -= avg;
    }

    // Применяем окно Хэмминга
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    // Выполняем FFT
    FFT.compute(FFT_FORWARD);

    // Преобразуем в амплитуды
    FFT.complexToMagnitude();

    // Вычисляем амплитуды в полосах
    calculateBands();
}

// Вычисление амплитуд в полосах
void AudioAnalyzer::calculateBands() {
    int step = (SAMPLES / 2) / MATRIX_WIDTH;
    if (step <= 0) {
        step = 1; // Устанавливаем минимальное значение
    }

    for (int b = 0; b < MATRIX_WIDTH; b++) {
        double sum = 0;
        int from = b * step;
        int to = (b + 1) * step;

        for (int i = from; i < to; i++) {
            sum += vReal[i];
        }

        sum /= (to - from);

        if (sum < NOISE_THRESHOLD) {
            sum = 0;
        }

        if (sensitivityReduction > 0) {
            sum /= sensitivityReduction;
        }

        if (b < MATRIX_WIDTH / 3) {
            sum *= lowFreqGain;
        } else if (b < 2 * MATRIX_WIDTH / 3) {
            sum *= midFreqGain;
        } else {
            sum *= highFreqGain;
        }

        // Ограничиваем значения
        sum = constrain(sum, 0, 400);
        bands[b] = (uint16_t)sum;

        // Постепенное уменьшение maxAmplitude
        maxAmplitude = max(maxAmplitude * 0.99, 1.0);
        if (bands[b] > maxAmplitude) {
            maxAmplitude = bands[b];
        }
    }
}

