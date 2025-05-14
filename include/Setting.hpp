#pragma once
#include <string>

enum class SettingType {
    BOOL,
    INT,
    FLOAT,
    STRING
};

struct Setting {
    std::string name;         // Ключ настройки
    std::string label;        // Отображаемое имя
    SettingType type;         // Тип значения
    void* value;              // Значение (указатель на данные любого типа)
    float minValue = 0.0f;    // Минимум (для числовых)
    float maxValue = 1.0f;    // Максимум (для числовых)
    float step = 1.0f;        // Шаг (для числовых)

    Setting(const std::string& name,
            const std::string& label,
            SettingType type,
            void* value,
            float minValue = 0.0f,
            float maxValue = 1.0f,
            float step = 1.0f)
        : name(name), label(label), type(type), value(value),
          minValue(minValue), maxValue(maxValue), step(step) {}
};


