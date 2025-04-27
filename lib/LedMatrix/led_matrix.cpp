#include "led_matrix.hpp"

// Конструктор
LedMatrix::LedMatrix() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.setBrightness(BRIGHTNESS); // Устанавливаем начальную яркость из config.hpp
}

// Преобразование координат (сверху вниз, слева направо)
int LedMatrix::XY(int x, int y) {
    return x * height + y;
}

// Очистка матрицы
void LedMatrix::clear() {
    for (int i = 0; i < width * height; i++) {
        leds[i] = CRGB::Black;
    }
}

// Установка цвета светодиода
void LedMatrix::setPixel(int x, int y, const CRGB& color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return; // Игнорируем некорректные координаты
    }
    leds[XY(x, y)] = color;
}

// Установка яркости
void LedMatrix::setBrightness(uint8_t brightness) {
    FastLED.setBrightness(brightness);
}

// Обновление матрицы
void LedMatrix::update() {
    FastLED.show();
}

// Доступ к массиву светодиодов
CRGB* LedMatrix::getLeds() {
    return leds;
}