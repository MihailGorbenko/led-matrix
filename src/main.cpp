#include <Arduino.h>
#include "audio_analyzer.hpp"
#include "led_matrix.hpp"
#include "sound_animator.hpp"
#include "config.hpp" // Подключаем файл конфигурации
#include <nvs_flash.h>

// Создаём объекты
LedMatrix ledMatrix;
SoundAnimator soundAnimator(ledMatrix); 
MatrixTask* currentMatrixTask = &soundAnimator; // Указатель на задачу матрицы


void setup() {
    Serial.begin(115200);

    // Инициализация NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    pinMode(MIC_PIN, INPUT);
    analogReadResolution(12); 
    analogSetAttenuation(ADC_11db);

    ledMatrix.begin(); // Инициализация матрицы
    ledMatrix.setBrightness(BRIGHTNESS);
   
    // Запускаем анимацию
    soundAnimator.initializeAudioAnalyzer(); // Инициализация AudioAnalyzer
    soundAnimator.setPulsingRingAnimation(); // Установка анимации пульсирующего кольца

    // Запускаем задачу для анимации
    currentMatrixTask->startTask();
}

void loop() {

    delay(1000); // Задержка в 1 секунду для предотвращения перегрузки Serial

}
