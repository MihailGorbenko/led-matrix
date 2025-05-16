#pragma once
#include "SerializableSetting.hpp"
#include "ISetting.h"
#include <Preferences.h>
#include <cstring>

template<typename T>
struct PersistantSetting : public SerializableSetting<T>, public ISetting {
    const char* const nvsNamespace;

    using SerializableSetting<T>::name;
    using SerializableSetting<T>::label;
    using SerializableSetting<T>::type;
    using SerializableSetting<T>::value;
    using SerializableSetting<T>::defaultValue;
    using SerializableSetting<T>::minValue;
    using SerializableSetting<T>::maxValue;
    using SerializableSetting<T>::step;
    using SerializableSetting<T>::toJSON;
    using SerializableSetting<T>::fromJSON;
    using SerializableSetting<T>::toJsonValue;
    using SerializableSetting<T>::fromJsonValue;
    using SerializableSetting<T>::reset;

    PersistantSetting(
        const char* name,
        const char* label,
        T* value,
        const char* ns = "settings",
        T defaultValue = T(),
        T minValue = T(),
        T maxValue = T(),
        T step = T()
    )
        : SerializableSetting<T>(name, label, value, minValue, maxValue, step, defaultValue)
        , nvsNamespace(ns)
    {}

    static void trimName(char* dst, const char* src) {
        if (!src) { dst[0] = '\0'; return; }
        strncpy(dst, src, 15);
        dst[15] = '\0';
    }

    // --- Реализация ISetting ---

    const char* getName() const override { return this->name; }

    JsonVariant toJsonValue() const override {
        return SerializableSetting<T>::toJsonValue();
    }

    bool fromJsonValue(JsonVariant val) override {
        return SerializableSetting<T>::fromJsonValue(val);
    }

    void toJSON(JsonObject& obj) const override {
        SerializableSetting<T>::toJSON(obj);
    }

    void getJsonSchema(JsonObject& obj) const override {
        SerializableSetting<T>::getJsonSchema(obj);
    }

    bool fromJSON(const JsonObject& obj) override {
        SerializableSetting<T>::fromJSON(obj);
        saveToNVS(); // ← сохраняем в NVS после применения значения из JSON
        return true;
    }

    bool saveToNVS() const override {
        char trimmedName[16];
        trimName(trimmedName, this->name);
        Preferences prefs;
        Serial.printf("[NVS] saveToNVS: ns='%s', key='%s', value=", nvsNamespace, trimmedName);
        bool ok = false;
        if (!prefs.begin(nvsNamespace, false)) {
            Serial.println("FAILED: prefs.begin()");
            return false;
        }
        if constexpr (std::is_same<T, int>::value) {
            int v = this->value ? *this->value : this->defaultValue;
            Serial.println(v);
            ok = prefs.putInt(trimmedName, v);
        } else if constexpr (std::is_same<T, float>::value) {
            float v = this->value ? *this->value : this->defaultValue;
            Serial.println(v);
            ok = prefs.putFloat(trimmedName, v);
        } else if constexpr (std::is_same<T, bool>::value) {
            bool v = this->value ? *this->value : this->defaultValue;
            Serial.println(v);
            ok = prefs.putBool(trimmedName, v);
        }
        prefs.end();
        Serial.printf("[NVS] saveToNVS result: %s\n", ok ? "OK" : "FAIL");
        return ok;
    }

    bool loadFromNVS() override {
        char trimmedName[16];
        trimName(trimmedName, this->name);
        Preferences prefs;
        Serial.printf("[NVS] loadFromNVS: ns='%s', key='%s' ... ", nvsNamespace, trimmedName);
        if (!prefs.begin(nvsNamespace, true)) {
            Serial.println("FAILED: prefs.begin()");
            return false;
        }
        bool found = false;
        if (prefs.isKey(trimmedName)) {
            if constexpr (std::is_same<T, int>::value) {
                int v = prefs.getInt(trimmedName, this->defaultValue);
                if (this->value) *this->value = v;
                Serial.printf("found int: %d\n", v);
                found = true;
            } else if constexpr (std::is_same<T, float>::value) {
                float v = prefs.getFloat(trimmedName, this->defaultValue);
                if (this->value) *this->value = v;
                Serial.printf("found float: %f\n", v);
                found = true;
            } else if constexpr (std::is_same<T, bool>::value) {
                bool v = prefs.getBool(trimmedName, this->defaultValue);
                if (this->value) *this->value = v;
                Serial.printf("found bool: %s\n", v ? "true" : "false");
                found = true;
            }
        } else {
            Serial.println("not found");
        }
        prefs.end();
        return found;
    }

    bool resetToDefaultAndSave() override {
        Serial.printf("[NVS] resetToDefaultAndSave: %s\n", this->name);
        this->reset();
        return saveToNVS();
    }
};
