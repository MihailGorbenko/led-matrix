#pragma once
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../Animator/animator.hpp"
#include "../LedMatrix/led_matrix.hpp"
#include "../AudioAnalyzer/audio_analyzer.hpp"

class AppWebServer {
public:
    AppWebServer(Animator* animator, LedMatrix* matrix, AudioAnalyzer* analyzer);

    void begin(uint16_t port = 80);
    void handleClient();

private:
    WebServer server;
    Animator* animator;
    LedMatrix* matrix;
    AudioAnalyzer* analyzer;

    void handleRoot();
    void handleGetConfig();
    void handleSetConfig();
    void handleGetSchema();
    void handleNotFound();
    void handleResetConfig();
    void handleResetAnimation();
    void handleResetMatrix();
    void handleResetAudioAnalyzer();
    void handleFavicon();
    void handleAppleTouchIcon();

    // Добавьте заглушки для часто запрашиваемых файлов
    void handleManifest();
    void handleRobots();
    void handleBrowserConfig();
    void handleSafariPinnedTab();
};