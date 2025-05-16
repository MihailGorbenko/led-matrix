#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include <nvs_flash.h>
#include "LedMatrix/led_matrix.hpp"

LedMatrix* matrix = nullptr;

unsigned long lastChange = 0;
bool isMax = false;

void setup() {
    Serial.begin(115200);
    delay(1000);

    matrix = new LedMatrix();
    matrix->begin();
      for (int x = 0; x < matrix->getWidth(); ++x) {
                for (int y = 0; y < matrix->getHeight(); ++y) {
                    matrix->setPixel(x, y, CRGB::White);
                }
            }
    matrix->update();

    
}

void loop() {
   
    unsigned long now = millis();
    if (now - lastChange > 10000) { // каждые 30 секунд
        lastChange = now;

        if (!isMax) {
            // Установить яркость 255 через JSON
            DynamicJsonDocument doc(64);
            JsonObject obj = doc.to<JsonObject>();
            obj["brightness"]["value"] = 200;
            matrix->fromJSON(obj);
            // Зажечь все пиксели белым
            for (int x = 0; x < matrix->getWidth(); ++x) {
                for (int y = 0; y < matrix->getHeight(); ++y) {
                    matrix->setPixel(x, y, CRGB::White);
                }
            }
            
            Serial.println("Яркость установлена на 255");
        } else {
            // Сбросить к значениям по умолчанию
            matrix->resetConfig();
            // Зажечь все пиксели белым
            for (int x = 0; x < matrix->getWidth(); ++x) {
                for (int y = 0; y < matrix->getHeight(); ++y) {
                    matrix->setPixel(x, y, CRGB::White);
                }
            }
            
            Serial.println("Яркость сброшена к default");
        }
        isMax = !isMax;

        // Вывести схему LedMatrix
        DynamicJsonDocument doc(128);
        JsonObject schema = doc.to<JsonObject>();
        matrix->getJsonSchema(schema);
        Serial.println("=== JSON Schema ===");
        serializeJsonPretty(doc, Serial);
        Serial.println();
    }
}

