#ifndef LED_MATRIX_HPP
#define LED_MATRIX_HPP

#include <FastLED.h>
#include "config.hpp"
#include "async_serial.hpp"

class LedMatrix {
private:
    CRGB leds[NUM_LEDS];
    int width = MATRIX_WIDTH;
    int height = MATRIX_HEIGHT;
    AsyncSerial& serial;

public:
    LedMatrix(AsyncSerial& asyncSerial); // Конструктор (без инициализации FastLED)
    
    void begin();                        // Явная инициализация FastLED (в setup)
    void clear();                        // Очистка матрицы и show()
    void setPixel(int x, int y, const CRGB& color); // Установка цвета
    void setBrightness(uint8_t brightness);         // Установка яркости
    void update();                       // Применить изменения
    void off();                          // Очистить и выключить
    CRGB* getLeds();                     // Доступ к массиву
    int XY(int x, int y);                // Преобразование координат
};

#endif // LED_MATRIX_HPP
