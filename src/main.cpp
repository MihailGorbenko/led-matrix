#include <Arduino.h>
#include "audio_analyzer.hpp"
#include "led_matrix.hpp"
#include "sound_animator.hpp"
#include "config.hpp" // Подключаем файл конфигурации

// Создаём объекты
LedMatrix ledMatrix;
AudioAnalyzer audioAnalyzer;
SoundAnimator soundAnimator(ledMatrix, audioAnalyzer);

void setup() {
    Serial.begin(115200);
    pinMode(MIC_PIN, INPUT);

    // Устанавливаем яркость матрицы
    ledMatrix.setBrightness(BRIGHTNESS);

    // Запускаем анимацию
    soundAnimator.startColorAmplitude();

    // Запускаем задачу для анимации
    soundAnimator.startTask();
}

void loop() {
    // Основной цикл остаётся пустым, так как анимация выполняется в задаче
}