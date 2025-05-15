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

    const char* const name;   // Ключ настройки
    const char* const label;  // Отображаемое имя
    SettingType type;         // Тип значения (вычисляется автоматически)
    T* value;                 // Указатель на данные нужного типа
    const T defaultValue;     // Значение по умолчанию (const)
    const T minValue;         // Минимум (const)
    const T maxValue;         // Максимум (const)
    const T step;             // Шаг (const)

    static constexpr SettingType deduceType() {
        return std::is_same<T, bool>::value  ? SettingType::BOOL  :
               std::is_same<T, int>::value   ? SettingType::INT   :
               std::is_same<T, float>::value ? SettingType::FLOAT :
                                               SettingType::INT;
    }

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
        if (type == SettingType::BOOL) {
            // Для BOOL диапазон и шаг не нужны, сбрасываем
            const_cast<T&>(this->minValue) = T();
            const_cast<T&>(this->maxValue) = T();
            const_cast<T&>(this->step) = T();
        }
    }

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


