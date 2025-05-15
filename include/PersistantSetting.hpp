#pragma once
#include "SerializableSetting.hpp"
#include <Preferences.h>
#include <cstring>

template<typename T>
struct PersistantSetting : public SerializableSetting<T> {
    const char* const nvsNamespace;

    PersistantSetting(const char* name, const char* label, T* value, const char* ns = "settings", T defaultValue = T())
        : SerializableSetting<T>(name, label, value, defaultValue), nvsNamespace(ns) {}

    // Вспомогательная функция: обрезка имени до 15 символов
    static void trimName(char* dst, const char* src) {
        if (!src) { dst[0] = '\0'; return; }
        strncpy(dst, src, 15);
        dst[15] = '\0';
    }

    // Сохранить значение в NVS (Preferences)
    bool saveToNVS() const {
        char trimmedName[16];
        trimName(trimmedName, this->name);
        Preferences prefs;
        if (!prefs.begin(nvsNamespace, false)) return false;
        bool ok = false;
        if constexpr (std::is_same<T, int>::value) {
            ok = prefs.putInt(trimmedName, this->value ? *this->value : this->defaultValue);
        } else if constexpr (std::is_same<T, float>::value) {
            ok = prefs.putFloat(trimmedName, this->value ? *this->value : this->defaultValue);
        } else if constexpr (std::is_same<T, bool>::value) {
            ok = prefs.putBool(trimmedName, this->value ? *this->value : this->defaultValue);
        }
        prefs.end();
        return ok;
    }

    // Загрузить значение из NVS (Preferences)
    bool loadFromNVS() {
        char trimmedName[16];
        trimName(trimmedName, this->name);
        Preferences prefs;
        if (!prefs.begin(nvsNamespace, true)) return false;
        bool found = false;
        if (prefs.isKey(trimmedName)) {
            if constexpr (std::is_same<T, int>::value) {
                int v = prefs.getInt(trimmedName, this->defaultValue);
                if (this->value) *this->value = v;
                found = true;
            } else if constexpr (std::is_same<T, float>::value) {
                float v = prefs.getFloat(trimmedName, this->defaultValue);
                if (this->value) *this->value = v;
                found = true;
            } else if constexpr (std::is_same<T, bool>::value) {
                bool v = prefs.getBool(trimmedName, this->defaultValue);
                if (this->value) *this->value = v;
                found = true;
            }
        }
        prefs.end();
        return found;
    }

    // Сбросить значение к умолчанию и сохранить в NVS
    bool resetToDefaultAndSave() {
        this->reset();         // сбросить к defaultValue
        return saveToNVS();    // сохранить в NVS
    }
};