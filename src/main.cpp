#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "config.hpp"
#include <nvs_flash.h>
#include "LedMatrix/led_matrix.hpp"
#include "AudioAnalyzer/audio_analyzer.hpp"
#include "Animator/animator.hpp"
#include "Animations/ColorAmplitude/color_amplitude.hpp"
#include "Animations/StarrySky/starry_sky.hpp"
#include "AppWebServer/app_web_server.hpp"
#include <SPIFFS.h>

LedMatrix matrix;
AudioAnalyzer analyzer;
Animator animator(&matrix, &analyzer);
AppWebServer webServer(&animator, &matrix, &analyzer);

// Подключение к Wi-Fi с ручным IP
void initWiFiConnection() {
    Serial.printf("[WiFi] Connecting to SSID: %s\n", WIFI_SSID);

    // Запрашиваем статический IP
    IPAddress local_IP(WIFI_LOCAL_IP);
    IPAddress gateway(WIFI_GATEWAY);
    IPAddress subnet(WIFI_SUBNET);
    IPAddress dns(WIFI_DNS);

    if (!WiFi.config(local_IP, gateway, subnet, dns)) {
        Serial.println("[WiFi] Failed to configure static IP");
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000; // 10 секунд

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("[WiFi] Connected! IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("[WiFi] Connection failed.");
    }
}

void setup() {
    Serial.begin(115200);

    // --- ВАЖНО: инициализация SPIFFS ---
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        while (1); // Остановить выполнение, если SPIFFS не смонтирован
    }

    // Подключение к Wi-Fi
    initWiFiConnection();

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

    animator.addAnimation(new ColorAmplitudeAnimation());
    animator.addAnimation(new StarrySkyAnimation());

    animator.begin();

    // --- Запуск веб-сервера ---
    webServer.begin(80);
}

void loop() {
    webServer.handleClient();
    delay(10);
}