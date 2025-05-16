#pragma once
#include <ArduinoJson.h>

class IConfigurable {
public:
    // Загрузка всех настроек из NVS
    virtual bool loadConfig() = 0;

    // Сохранение всех текущих настроек в NVS
    virtual bool saveConfig() const = 0;

    // Сброс всех настроек к значениям по умолчанию и сохранение в NVS
    virtual bool resetConfig() = 0;

    // Сериализация всех настроек в JSON
    virtual void toJSON(JsonObject& json) const = 0;

    // Загрузка настроек из JSON
    virtual bool fromJSON(const JsonObject& json) = 0;

    // Получение JSON-схемы для всех настроек
    virtual void getJsonSchema(JsonObject& json) const = 0;

    // Виртуальный деструктор
    virtual ~IConfigurable() = default;
};

