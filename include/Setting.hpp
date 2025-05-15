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
    SettingType type;      // Тип значения (вычисляется автоматически)
    T* value;              // Указатель на данные нужного типа
    T defaultValue = T();  // Значение по умолчанию
    T minValue = T();      // Минимум (для числовых)
    T maxValue = T();      // Максимум (для числовых)
    T step = T();          // Шаг (для числовых)

    // Вспомогательная функция для вычисления SettingType по T
    static constexpr SettingType deduceType() {
        return std::is_same<T, bool>::value  ? SettingType::BOOL  :
               std::is_same<T, int>::value   ? SettingType::INT   :
               std::is_same<T, float>::value ? SettingType::FLOAT :
                                               SettingType::INT; // fallback
    }

    // Универсальный конструктор
    Setting(const char* name,
            const char* label,
            T* value,
            T minValue = T(),
            T maxValue = T(),
            T step = T(),
            T defaultValue = T())
        : name(name), label(label), type(deduceType()), value(value),
          defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step)
    {
        // Для BOOL диапазон и шаг не нужны, сбрасываем
        if (type == SettingType::BOOL) {
            this->minValue = T();
            this->maxValue = T();
            this->step = T();
        }
    }

    // Сброс значения к значению по умолчанию
    void reset() {
        if (value) {
            *value = defaultValue;
        }
    }

    bool setValue(T newValue) {
        if (type != SettingType::BOOL && (newValue < minValue || newValue > maxValue)) return false;
        if (value) *value = newValue;
        return true;
    }

    T getValue() const { return value ? *value : defaultValue; }
};


