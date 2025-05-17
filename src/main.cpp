#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include <nvs_flash.h>
#include "AudioAnalyzer/audio_analyzer.hpp"

AudioAnalyzer* analyzer = nullptr;

unsigned long lastSettingsChange = 0;
unsigned long lastVisualization = 0;
bool toggle = false;

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

    analyzer = new AudioAnalyzer();
    analyzer->begin();

    Serial.println("AudioAnalyzer инициализирован.");
}

void loop() {
    unsigned long now = millis();

    // --- Меняем настройки через JSON каждые 10 секунд ---
    static unsigned long lastJsonChange = 0;
    static bool customSettings = false;
    if (now - lastJsonChange > 10000) {
        lastJsonChange = now;
        if (!customSettings) {
            // Применяем пользовательские настройки через JSON
            StaticJsonDocument<256> doc;
            doc["sensitivityReduction"]["value"] = 8;
            doc["alpha"]["value"] = 0.7;
            doc["lowFreqGain"]["value"] = 2;
            doc["midFreqGain"]["value"] = 1.5;
            doc["highFreqGain"]["value"] = 3;
            doc["fMin"]["value"] = 100;
            doc["fMax"]["value"] = 8000;
            doc["noiseThresholdRatio"]["value"] = 0.4;
            doc["bandDecay"]["value"] = 0.9;
            doc["bandCeiling"]["value"] = 500;
            analyzer->fromJSON(doc.as<JsonObject>());
            Serial.println(">>> Применены пользовательские настройки через JSON");
        } else {
            // Сброс до заводских настроек
            analyzer->resetConfig();
            Serial.println(">>> Сброс до заводских настроек");
        }

        customSettings = !customSettings;
    }

    // --- Визуализируем спектр ---
    if (now - lastVisualization > 100) {
        lastVisualization = now;
        analyzer->processAudio();
        static uint16_t heights[MATRIX_WIDTH];
        analyzer->getNormalizedHeights(heights, MATRIX_HEIGHT);

        Serial.print("\033[2J\033[H");
        for (int row = MATRIX_HEIGHT - 1; row >= 0; --row) {
            for (int col = 0; col < MATRIX_WIDTH; ++col) {
                if (heights[col] > row) Serial.print("# ");
                else Serial.print("  ");
            }
            Serial.println();
        }
        Serial.println();
    }
    delay(10);
}

