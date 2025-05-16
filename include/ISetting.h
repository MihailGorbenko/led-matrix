#pragma once
#include <ArduinoJson.h>

struct ISetting {
    const char* name;

    virtual JsonVariant toJsonValue() const = 0;
    virtual bool fromJsonValue(JsonVariant val) = 0;

    virtual void toJSON(JsonObject& obj) const = 0;
    virtual bool fromJSON(const JsonObject& obj) = 0;

    virtual bool saveToNVS() const = 0;
    virtual bool loadFromNVS() = 0;
    virtual bool resetToDefaultAndSave() = 0;

    virtual ~ISetting() = default;
};
