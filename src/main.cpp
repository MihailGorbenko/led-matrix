#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include <nvs_flash.h>
#include "LedMatrix/led_matrix.hpp"
#include "AudioAnalyzer/audio_analyzer.hpp"
#include "Animator/animator.hpp"
#include "Animations/ColorAmplitude/color_amplitude.cpp" // Лучше .hpp, если есть

LedMatrix matrix;
AudioAnalyzer analyzer;
Animator animator(&matrix, &analyzer);


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

    matrix.begin();
    analyzer.begin();

    // Добавляем анимацию
    animator.addAnimation(new ColorAmplitudeAnimation());
    animator.begin();

    // Установить чёрный цвет (0) через JSON
    StaticJsonDocument<64> doc2;
    doc2["color"]["value"] = 0x000000; // Чёрный
    animator.getCurrentAnimation()->fromJSON(doc2.as<JsonObject>());
    Serial.println("[TEST] Применён JSON-конфиг для colorAmplitude (чёрный)");

    // Имитация выбора анимации через JSON (как будто пользователь выбрал её в веб-интерфейсе)
    StaticJsonDocument<64> doc3;
    doc3["currentAnimationType"] = "colorAmplitude";
    animator.fromJSON(doc3.as<JsonObject>());
    Serial.println("[TEST] Выбрана анимация colorAmplitude");


}

void loop() {
   
    delay(10);
}

