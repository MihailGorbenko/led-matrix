#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <FastLED.h>
#include "config.hpp" // Подключаем файл конфигурации

class LedMatrix {
private:
    static const int width = MATRIX_WIDTH;  // Ширина матрицы
    static const int height = MATRIX_HEIGHT; // Высота матрицы
    CRGB leds[NUM_LEDS]; // Инкапсулированный массив светодиодов

public:
    // Конструктор
    LedMatrix();

    // Преобразование координат
    int XY(int x, int y);

    // Очистка матрицы
    void clear();

    // Установка цвета светодиода
    void setPixel(int x, int y, const CRGB& color);

    // Установка яркости
    void setBrightness(uint8_t brightness);

    // Обновление матрицы
    void update();

    // Доступ к массиву светодиодов
    CRGB* getLeds();
};

#endif // LED_MATRIX_H