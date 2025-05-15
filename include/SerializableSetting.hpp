#pragma once
#include "Setting.hpp"
#include <ArduinoJson.h>

// Шаблонная структура, расширяющая Setting<T> методами сериализации
template<typename T>
struct SerializableSetting : public Setting<T> {
    using Setting<T>::name;
    using Setting<T>::label;
    using Setting<T>::type;
    using Setting<T>::value;
    using Setting<T>::defaultValue;
    using Setting<T>::minValue;
    using Setting<T>::maxValue;
    using Setting<T>::step;

    using Setting<T>::Setting; // Наследуем конструкторы

    // Сериализация в JSON: заполняет переданный JsonObject
    void toJSON(JsonObject& obj) const {
        obj["name"] = name;
        obj["label"] = label;
        obj["value"] = value ? *value : defaultValue;
        obj["default"] = defaultValue;
        obj["min"] = (type != SettingType::BOOL) ? minValue : 0;
        obj["max"] = (type != SettingType::BOOL) ? maxValue : 0;
        obj["step"] = (type != SettingType::BOOL) ? step : 0;
    }

    // Десериализация из JSON: обновляет все поля
    void fromJSON(const JsonObject& obj) {
        // name, label и type не меняем!
        if (value && obj.containsKey("value")) {
            T v = obj["value"].as<T>();
            if (type == SettingType::BOOL || (v >= minValue && v <= maxValue)) {
                *value = v;
            } else if (v < minValue) {
                *value = minValue;
            } else if (v > maxValue) {
                *value = maxValue;
            }
        }
        if (obj.containsKey("default")) defaultValue = obj["default"].as<T>();
        if (type != SettingType::BOOL) {
            if (obj.containsKey("min"))  minValue = obj["min"].as<T>();
            if (obj.containsKey("max"))  maxValue = obj["max"].as<T>();
            if (obj.containsKey("step")) step = obj["step"].as<T>();
        }
    }
};

