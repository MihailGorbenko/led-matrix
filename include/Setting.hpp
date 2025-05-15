#pragma once
#include <cstddef>
#include <type_traits>
#include <cstring>

enum class SettingType {
    BOOL,
    INT,
    FLOAT,
};

// Преобразование SettingType <-> строка
inline const char* settingTypeToString(SettingType type) {
    switch (type) {
        case SettingType::BOOL:  return "bool";
        case SettingType::INT:   return "int";
        case SettingType::FLOAT: return "float";
        default:                 return "unknown";
    }
}

inline SettingType settingTypeFromString(const char* str) {
    if (strcmp(str, "bool") == 0)  return SettingType::BOOL;
    if (strcmp(str, "int") == 0)   return SettingType::INT;
    if (strcmp(str, "float") == 0) return SettingType::FLOAT;
    return SettingType::INT; // дефолт
}

// Шаблонная структура Setting для любого типа T, имена и метки — char*
template<typename T>
struct Setting {
    static_assert(
        std::is_same<T, bool>::value ||
        std::is_same<T, int>::value  ||
        std::is_same<T, float>::value,
        "Setting<T>: T должен быть bool, int или float"
    );

    const char* name;      // Ключ настройки
    const char* label;     // Отображаемое имя
    SettingType type;      // Тип значения
    T* value;              // Указатель на данные нужного типа
    const T defaultValue = T(); // Значение по умолчанию
    T minValue = T();      // Минимум (для числовых)
    T maxValue = T();      // Максимум (для числовых)
    T step = T();         // Шаг (для числовых)

    // Универсальный конструктор
    Setting(const char* name,
            const char* label,
            SettingType type,
            T* value,
            T minValue = T(),
            T maxValue = T(),
            T step = T(),
            T defaultValue = T())
        : name(name), label(label), type(type), value(value),
          defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step)
    {
        // Для BOOL диапазон и шаг не нужны, сбрасываем
        if (type == SettingType::BOOL) {
            this->minValue = T();
            this->maxValue = T();
            this->step = T();
        }
    }
};


