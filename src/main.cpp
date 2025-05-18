#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include <nvs_flash.h>
#include "LedMatrix/led_matrix.hpp"
#include "AudioAnalyzer/audio_analyzer.hpp"
#include "Animator/animator.hpp"
#include "Animations/ColorAmplitude/color_amplitude.hpp" // Лучше .hpp, если есть
#include "Animations/StarrySky/starry_sky.hpp" // добавить include

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

    // Добавляем анимации
    animator.addAnimation(new ColorAmplitudeAnimation());
    animator.addAnimation(new StarrySkyAnimation());

    animator.begin();

    // Пример выбора starrySky через JSON:
    StaticJsonDocument<128> doc;
    doc["starrySky"]["starColor"]["value"] = 0xC800FF; // сиреневый цвет
    doc["starrySky"]["starsPercent"]["value"] = 50;
    doc["currentAnimationType"] = "starrySky";
    animator.fromJSON(doc.as<JsonObject>());
    Serial.println("[TEST] Выбрана анимация starrySky");

    // Компонуем схемы в один JSON-объект
    StaticJsonDocument<4096> allSchema;
    JsonObject animatorSchema = allSchema.createNestedObject("animator");
    animator.getJsonSchema(animatorSchema);

    JsonObject matrixSchema = allSchema.createNestedObject("ledMatrix");
    matrix.getJsonSchema(matrixSchema);

    JsonObject analyzerSchema = allSchema.createNestedObject("audioAnalyzer");
    analyzer.getJsonSchema(analyzerSchema);

    // Выводим общий JSON-объект в Serial
    Serial.println("[SCHEMA] All modules:");
    serializeJsonPretty(allSchema, Serial);
}

void loop() {
   
    delay(10);
}

