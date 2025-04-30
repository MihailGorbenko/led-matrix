#include "led_matrix.hpp"

// Конструктор — только сохраняем ссылки
LedMatrix::LedMatrix(AsyncSerial& asyncSerial) : serial(asyncSerial) {}

// Инициализация FastLED — вызывать в setup()
void LedMatrix::begin() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    clear(); // Очистка и show
}

// Преобразование координат (сверху вниз, слева направо)
int LedMatrix::XY(int x, int y) {
    return x * height + y;
}

// Очистка матрицы (fill_solid)
void LedMatrix::clear() {
    fill_solid(leds, width * height, CRGB::Black);
}

// Установка цвета
void LedMatrix::setPixel(int x, int y, const CRGB& color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    leds[XY(x, y)] = color;
}

// Установка яркости
void LedMatrix::setBrightness(uint8_t brightness) {
    FastLED.setBrightness(brightness);
}

// Обновление матрицы (показать)
void LedMatrix::update() {
    FastLED.show();
}

// Полное выключение (очистить + показать)
void LedMatrix::off() {
    clear();
    FastLED.show();
}

// Получить массив пикселей
CRGB* LedMatrix::getLeds() {
    return leds;
}
