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

    // Сериализация в JSON: возвращает JsonObject
    JsonObject toJSON(JsonDocument& doc) const {
        JsonObject obj = doc.to<JsonObject>();
        obj["name"] = name;
        obj["label"] = label;
        obj["type"] = settingTypeToString(type);
        obj["value"] = *value;
        obj["default"] = defaultValue;
        obj["min"] = (type != SettingType::BOOL) ? minValue : 0;
        obj["max"] = (type != SettingType::BOOL) ? maxValue : 0;
        obj["step"] = (type != SettingType::BOOL) ? step : 0;
        return obj;
    }

    // Десериализация из JSON: обновляет все поля
    void fromJSON(const JsonObject& obj) {
        if (obj.containsKey("name"))        name = obj["name"].as<const char*>();
        if (obj.containsKey("label"))       label = obj["label"].as<const char*>();
        if (obj.containsKey("type"))        type = settingTypeFromString(obj["type"]);
        if (obj.containsKey("value"))       *value = obj["value"].as<T>();
        if (obj.containsKey("default"))     defaultValue = obj["default"].as<T>();
        if (type != SettingType::BOOL) {
            if (obj.containsKey("min"))     minValue = obj["min"].as<T>();
            if (obj.containsKey("max"))     maxValue = obj["max"].as<T>();
            if (obj.containsKey("step"))    step = obj["step"].as<T>();
        }
    }
};

