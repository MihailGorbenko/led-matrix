#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <FastLED.h>
#include "config.hpp" // Подключаем файл конфигурации
#include "async_serial.hpp" // Подключаем класс AsyncSerial

class LedMatrix {
private:
    static const int width = MATRIX_WIDTH;  // Ширина матрицы
    static const int height = MATRIX_HEIGHT; // Высота матрицы
    CRGB leds[NUM_LEDS]; // Инкапсулированный массив светодиодов
    AsyncSerial& serial; // Добавляем член класса для хранения ссылки на AsyncSerial

public:
    // Конструктор
    LedMatrix(AsyncSerial& asyncSerial);
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

    void off();

    // Доступ к массиву светодиодов
    CRGB* getLeds();
};

#endif // LED_MATRIX_H