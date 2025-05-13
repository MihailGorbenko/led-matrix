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

// Переменные для управления анимацией
unsigned long lastAnimationChangeTime = 0; // Время последнего переключения анимации
const unsigned long animationInterval = 60 * 1000; // Интервал в 1 минуту
int currentAnimationIndex = 0; // Индекс текущей анимации

// Массив доступных цветов
const CRGB availableColors[] = {
    CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Cyan, CRGB::Magenta, CRGB::Orange, CRGB::Purple, CRGB::White
};
const int numColors = sizeof(availableColors) / sizeof(availableColors[0]); // Количество доступных цветов

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
    soundAnimator.init(); // Инициализация SoundAnimator
    soundAnimator.initializeAudioAnalyzer(); // Инициализация AudioAnalyzer
    soundAnimator.setPulsingRectangleSensitivity(0.9f);
    soundAnimator.setWaveSensitivity(1.0f); // Установка чувствительности
    soundAnimator.setStarrySkyMaxStars(50); // Максимальное количество звёзд
    soundAnimator.setStarrySkySensitivity(0.8f); // Чувствительность звёздного неба
    soundAnimator.setAnimation(AnimationType::StarrySky, CRGB::Green); // Установка начальной анимации


    // Запускаем задачу для анимации
    currentMatrixTask->startTask();
}

void loop() {

    // Проверяем, прошло ли 1 минута
    unsigned long currentTime = millis();
    if (currentTime - lastAnimationChangeTime >= animationInterval) {
        lastAnimationChangeTime = currentTime;


        // Генерируем случайный цвет из массива
        CRGB randomColor = availableColors[random(0, numColors)];

        // Переключаем анимацию
        currentAnimationIndex = (currentAnimationIndex + 1) % 4; // Переключаем между 4 анимациями

        switch (currentAnimationIndex) {
            case 0:
                soundAnimator.setAnimation(AnimationType::StarrySky, randomColor); // Звёздное небо
                Serial.println("Switched to Starry Sky Animation");
                break;
            case 1:
                soundAnimator.setAnimation(AnimationType::PulsingRectangle, randomColor); // Пульсирующий прямоугольник
                Serial.println("Switched to Pulsing Rectangle Animation");
                break;
            case 2:
                soundAnimator.setAnimation(AnimationType::ColorAmplitude, randomColor); // Амплитуда цвета
                Serial.println("Switched to Color Amplitude Animation");
                break;
            case 3:
                soundAnimator.setAnimation(AnimationType::Wave, randomColor); // Волна
                Serial.println("Switched to Wave Animation");
                break;
        }
    }

    delay(1000); // Задержка в 1 секунду для предотвращения перегрузки
}

