#ifndef LED_MATRIX_HPP
#define LED_MATRIX_HPP

#include <FastLED.h>
#include "config.hpp"
#include "PersistantSetting.hpp"
#include "../ConfigurableBase/configurable_base.hpp"

#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 9
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define DEF_BRIGHTNESS 64

/// Класс управления светодиодной матрицей с поддержкой конфигурирования и сохранения настроек
class LedMatrix : public ConfigurableBase {
private:
    CRGB leds[NUM_LEDS];
    int width = MATRIX_WIDTH;
    int height = MATRIX_HEIGHT;
    int brightness = DEF_BRIGHTNESS;

    PersistantSetting<int> brightnessSetting;

public:
    LedMatrix(); ///< Конструктор (без инициализации FastLED)
    void begin(); ///< Явная инициализация FastLED (в setup)
    void clear(); ///< Очистка матрицы и show()
    void setPixel(int x, int y, const CRGB& color); ///< Установка цвета пикселя
    void setBrightness(uint8_t brightness); ///< Установка яркости
    int getBrightness() const { return brightness; }
    void update(); ///< Применить изменения (FastLED.show)
    void off(); ///< Очистить и выключить
    CRGB* getLeds(); ///< Доступ к массиву пикселей
    int XY(int x, int y); ///< Преобразование координат

    int getWidth() const;
    int getHeight() const;
    int getNumLeds() const;
    /// Применить настройки из JSON-объекта, вернуть true при успехе
    bool fromJSON(const JsonObject& obj) override;
    /// Сбросить настройки к значениям по умолчанию, вернуть true при успехе
    bool resetConfig() override;
};

#endif // LED_MATRIX_HPP
