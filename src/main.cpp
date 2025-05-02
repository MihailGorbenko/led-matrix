#include <Arduino.h>
#include "audio_analyzer.hpp"
#include "led_matrix.hpp"
#include "sound_animator.hpp"
#include "config.hpp" // Подключаем файл конфигурации
#include <nvs_flash.h>

// Создаём объекты
LedMatrix ledMatrix;
AudioAnalyzer audioAnalyzer; // Создаём объект AudioAnalyzer
SoundAnimator soundAnimator(ledMatrix,audioAnalyzer); // Получаем ссылку на AudioAnalyzer
MatrixTask* currentMatrixTask = &soundAnimator; // Указатель на задачу матрицы


void setup() {
    Serial.begin(115200);
    pinMode(MIC_PIN, INPUT);
    analogReadResolution(12); 
    analogSetAttenuation(ADC_11db);

    ledMatrix.begin(); // Инициализация матрицы
    ledMatrix.setBrightness(BRIGHTNESS);

    audioAnalyzer.begin(); // Инициализация анализатора звука
    audioAnalyzer.setSensitivityReduction(5.0); // Настройка чувствительности
    audioAnalyzer.setLowFreqGain(0.8); // Настройка усиления низких частот      
    audioAnalyzer.setMidFreqGain(1.1); // Настройка усиления средних частот
    audioAnalyzer.setHighFreqGain(1.1); // Настройка усиления высоких частот

    // Запускаем анимацию
    soundAnimator.setColorAmplitudeAnimation();

    // Запускаем задачу для анимации
    currentMatrixTask->startTask();
}

void loop() {
    // Основной цикл остаётся пустым, так как анимация выполняется в задаче
    delay(1000); // Добавляем небольшую задержку для снижения нагрузки
   

}

// TODO:
//1. сделать вывод в serial из разных задач(нужно разобраться с последовательностью инициализации)
//2. сделать совмесное использование nvs flash между задачами
//
//
