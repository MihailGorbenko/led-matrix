#include "led_matrix.hpp"

LedMatrix::LedMatrix()
    : ConfigurableBase("LedMatrix", "Матрица"),
      brightnessSetting(
          "brightness", "Яркость", &brightness, "led_matrix", DEF_BRIGHTNESS, 0, 255, 1
      )
{
    registerSetting(&brightnessSetting);
}

void LedMatrix::begin() {
    Serial.println("[LedMatrix] Инициализация...");
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    // Загружаем настройки из NVS
    if (!loadConfig()) {
        Serial.println("[ERROR] Не удалось загрузить настройки LedMatrix из NVS");
    }
    // Применяем яркость из NVS (brightness уже обновлён)
    FastLED.setBrightness(brightness);
    clear();
    FastLED.show();
    Serial.println("[LedMatrix] Инициализация завершена.");
}

int LedMatrix::XY(int x, int y) {
    return x * height + y;
}

void LedMatrix::clear() {
    fill_solid(leds, width * height, CRGB::Black);
}

void LedMatrix::setPixel(int x, int y, const CRGB& color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        Serial.printf("[WARN] setPixel: координаты вне диапазона: x=%d, y=%d\n", x, y);
        return;
    }
    leds[XY(x, y)] = color;
}

void LedMatrix::setBrightness(uint8_t b) {
    brightness = b;
    FastLED.setBrightness(brightness);
}

void LedMatrix::update() {
    FastLED.show();
}

void LedMatrix::off() {
    clear();
    FastLED.show();
}

CRGB* LedMatrix::getLeds() {
    return leds;
}

int LedMatrix::getWidth() const {
    return width;
}

int LedMatrix::getHeight() const {
    return height;
}

int LedMatrix::getNumLeds() const {
    return width * height;
}

bool LedMatrix::fromJSON(const JsonObject& obj) {
    bool changed = ConfigurableBase::fromJSON(obj);
    FastLED.setBrightness(brightness);
    this->update();
    if (!changed) {
        Serial.println("[WARN] fromJSON: настройки не были изменены");
    }
    return changed;
}

bool LedMatrix::resetConfig() {
    bool ok = ConfigurableBase::resetConfig();
    FastLED.setBrightness(brightness);
    this->update();
    if (!ok) {
        Serial.println("[ERROR] resetConfig: сброс настроек не удался");
    }
    return ok;
}


