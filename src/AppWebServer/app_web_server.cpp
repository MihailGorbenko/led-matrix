#include "app_web_server.hpp"
#include <SPIFFS.h>

AppWebServer::AppWebServer(Animator* animator, LedMatrix* matrix, AudioAnalyzer* analyzer)
    : server(), animator(animator), matrix(matrix), analyzer(analyzer) {}

void AppWebServer::begin(uint16_t port) {
    server.on("/", [this]() { handleRoot(); });
    server.on("/api/config", HTTP_GET, [this]() { handleGetConfig(); });
    server.on("/api/config", HTTP_POST, [this]() { handleSetConfig(); });
    server.on("/api/schema", HTTP_GET, [this]() { handleGetSchema(); });
    server.on("/api/reset", HTTP_POST, [this]() { handleResetConfig(); });
    server.on("/api/reset/animation", HTTP_POST, [this]() { handleResetAnimation(); }); // добавьте эту строку
    server.onNotFound([this]() { handleNotFound(); });
    server.begin(port);
}

void AppWebServer::handleClient() {
    server.handleClient();
}

void AppWebServer::handleRoot() {
    // Отдаём index.html из SPIFFS/LittleFS, если он есть
    if (SPIFFS.begin(true)) {
        File file = SPIFFS.open("/index.html", "r");
        if (file) {
            server.streamFile(file, "text/html");
            file.close();
            return;
        }
    }
    // Если файла нет — отдаём заглушку
    server.send(200, "text/html", "<!DOCTYPE html><html><body><h1>index.html not found</h1></body></html>");
}

void AppWebServer::handleGetConfig() {
    StaticJsonDocument<4096> doc;
    JsonObject animatorObj = doc.createNestedObject("animator");
    animator->getJsonSchema(animatorObj); // ← Должно быть toJSON!
    JsonObject matrixObj = doc.createNestedObject("ledMatrix");
    matrix->getJsonSchema(matrixObj);
    JsonObject analyzerObj = doc.createNestedObject("audioAnalyzer");
    analyzer->getJsonSchema(analyzerObj);
    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
}

void AppWebServer::handleSetConfig() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"No JSON body\"}");
        return;
    }
    StaticJsonDocument<4096> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));
    if (err) {
        server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }
    if (doc.containsKey("animator")) animator->fromJSON(doc["animator"]);
    if (doc.containsKey("ledMatrix")) matrix->fromJSON(doc["ledMatrix"]);
    if (doc.containsKey("audioAnalyzer")) analyzer->fromJSON(doc["audioAnalyzer"]);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void AppWebServer::handleGetSchema() {
    StaticJsonDocument<8192> doc;
    JsonObject animatorObj = doc.createNestedObject("animator");
    animator->getJsonSchema(animatorObj);
    JsonObject matrixObj = doc.createNestedObject("ledMatrix");
    matrix->getJsonSchema(matrixObj);
    JsonObject analyzerObj = doc.createNestedObject("audioAnalyzer");
    analyzer->getJsonSchema(analyzerObj);
    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
}

void AppWebServer::handleResetConfig() {
    bool ok1 = animator->resetConfig();
    bool ok2 = matrix->resetConfig();
    bool ok3 = analyzer->resetConfig();
    if (ok1 && ok2 && ok3) {
        server.send(200, "application/json", "{\"status\":\"reset ok\"}");
    } else {
        server.send(500, "application/json", "{\"status\":\"reset failed\"}");
    }
}

void AppWebServer::handleResetAnimation() {
    if (!server.hasArg("name")) {
        server.send(400, "application/json", "{\"error\":\"No animation name\"}");
        return;
    }
    String name = server.arg("name");
    // Найти анимацию по moduleName
    for (auto* anim : animator->getAnimations()) {
        if (name == anim->getModuleName()) {
            bool ok = anim->resetConfig();
            if (ok) {
                server.send(200, "application/json", "{\"status\":\"reset ok\"}");
            } else {
                server.send(500, "application/json", "{\"status\":\"reset failed\"}");
            }
            return;
        }
    }
    server.send(404, "application/json", "{\"error\":\"Animation not found\"}");
}

void AppWebServer::handleNotFound() {
    server.send(404, "application/json", "{\"error\":\"Not found\"}");
}