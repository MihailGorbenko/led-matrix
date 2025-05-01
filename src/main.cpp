#include <Arduino.h>
#include "audio_analyzer.hpp"
#include "led_matrix.hpp"
#include "sound_animator.hpp"
#include "async_serial.hpp"
#include "config.hpp" // Подключаем файл конфигурации
#include <nvs_flash.h>

// Создаём объекты
AsyncSerial asyncSerial; // Создаём объект для асинхронного вывода в Serial
LedMatrix ledMatrix(asyncSerial);
AudioAnalyzer audioAnalyzer(asyncSerial); // Создаём объект AudioAnalyzer
SoundAnimator soundAnimator(ledMatrix,asyncSerial,audioAnalyzer); // Получаем ссылку на AudioAnalyzer
MatrixTask* currentMatrixTask = &soundAnimator; // Указатель на задачу матрицы


void setup() {
    Serial.begin(115200);
    pinMode(MIC_PIN, INPUT);
    analogSetAttenuation(ADC_11db);

    // Устанавливаем яркость матрицы
    ledMatrix.setBrightness(BRIGHTNESS);

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
