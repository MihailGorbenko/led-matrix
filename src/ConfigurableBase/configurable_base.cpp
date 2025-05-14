#include "configurable_base.hpp"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include <Preferences.h>
#include <algorithm>
#include <cctype>

// Вспомогательная функция для генерации namespace из имени модуля
static std::string makeNvsNamespace(const std::string& moduleName) {
    std::string ns;
    for (char c : moduleName) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            ns += c;
        } else if (c == ' ' || c == '-') {
            ns += '_';
        }
        if (ns.length() >= 15) break; // Ограничение NVS
    }
    if (ns.empty()) ns = "module";
    return ns;
}

// Фильтрация имени ключа для NVS
static std::string filterKeyName(const std::string& key) {
    std::string filtered;
    for (char c : key) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            filtered += c;
        } else if (c == ' ' || c == '-') {
            filtered += '_';
        }
        if (filtered.length() >= 15) break; // Ограничение NVS
    }
    if (filtered.empty()) filtered = "key";
    return filtered;
}

// Конструктор: инициализация дефолтных настроек и попытка загрузки из NVS
ConfigurableBase::ConfigurableBase() {
}

// Получить текущий список настроек
const std::vector<Setting>& ConfigurableBase::getSettings() const {
    return settings;
}

// Сохранить все настройки в NVS
void ConfigurableBase::saveSettingsToNVS() {
    std::string nvsNamespace = makeNvsNamespace(getModuleName());
    Preferences prefs;
    Serial.printf("[%s] Saving settings to NVS namespace: %s\n", getModuleName().c_str(), nvsNamespace.c_str());
    prefs.begin(nvsNamespace.c_str(), false);
    for (const auto& setting : settings) {
        std::string key = filterKeyName(setting.name);
        switch (setting.type) {
            case SettingType::BOOL:
                prefs.putBool(key.c_str(), *(bool*)setting.value);
                Serial.printf("  Saved BOOL: %s = %d\n", key.c_str(), *(bool*)setting.value);
                break;
            case SettingType::INT:
                prefs.putInt(key.c_str(), *(int*)setting.value);
                Serial.printf("  Saved INT: %s = %d\n", key.c_str(), *(int*)setting.value);
                break;
            case SettingType::FLOAT:
                prefs.putFloat(key.c_str(), *(float*)setting.value);
                Serial.printf("  Saved FLOAT: %s = %f\n", key.c_str(), *(float*)setting.value);
                break;
            case SettingType::STRING:
                prefs.putString(key.c_str(), ((std::string*)setting.value)->c_str());
                Serial.printf("  Saved STRING: %s = %s\n", key.c_str(), ((std::string*)setting.value)->c_str());
                break;
        }
    }
    prefs.end();
    Serial.printf("[%s] Settings saved to NVS.\n", getModuleName().c_str());
}

// Загрузить все настройки из NVS (если есть)
void ConfigurableBase::loadSettingsFromNVS() {
    std::string nvsNamespace = makeNvsNamespace(getModuleName());
    Preferences prefs;
    Serial.printf("[%s] Loading settings from NVS namespace: %s\n", getModuleName().c_str(), nvsNamespace.c_str());
    prefs.begin(nvsNamespace.c_str(), true);
    for (auto& setting : settings) {
        std::string key = filterKeyName(setting.name);
        switch (setting.type) {
            case SettingType::BOOL:
                *(bool*)setting.value = prefs.getBool(key.c_str(), *(bool*)setting.value);
                Serial.printf("  Loaded BOOL: %s = %d\n", key.c_str(), *(bool*)setting.value);
                break;
            case SettingType::INT:
                *(int*)setting.value = prefs.getInt(key.c_str(), *(int*)setting.value);
                Serial.printf("  Loaded INT: %s = %d\n", key.c_str(), *(int*)setting.value);
                break;
            case SettingType::FLOAT:
                *(float*)setting.value = prefs.getFloat(key.c_str(), *(float*)setting.value);
                Serial.printf("  Loaded FLOAT: %s = %f\n", key.c_str(), *(float*)setting.value);
                break;
            case SettingType::STRING: {
                String str = prefs.getString(key.c_str(), ((std::string*)setting.value)->c_str());
                *(std::string*)setting.value = str.c_str();
                Serial.printf("  Loaded STRING: %s = %s\n", key.c_str(), str.c_str());
                break;
            }
        }
    }
    prefs.end();
    Serial.printf("[%s] Settings loaded from NVS.\n", getModuleName().c_str());
}

// Применить настройки из JSON и сохранить их в NVS
void ConfigurableBase::configureJson(const JsonObject& confJson) {
    for (auto& setting : settings) {
        if (confJson[setting.name.c_str()].isNull()) continue;
        switch (setting.type) {
            case SettingType::BOOL:
                *(bool*)setting.value = confJson[setting.name.c_str()].as<bool>();
                Serial.printf("  Set BOOL: %s = %d\n", setting.name.c_str(), *(bool*)setting.value);
                break;
            case SettingType::INT:
                *(int*)setting.value = confJson[setting.name.c_str()].as<int>();
                Serial.printf("  Set INT: %s = %d\n", setting.name.c_str(), *(int*)setting.value);
                break;
            case SettingType::FLOAT:
                *(float*)setting.value = confJson[setting.name.c_str()].as<float>();
                Serial.printf("  Set FLOAT: %s = %f\n", setting.name.c_str(), *(float*)setting.value);
                break;
            case SettingType::STRING:
                *(std::string*)setting.value = confJson[setting.name.c_str()].as<const char*>();
                Serial.printf("  Set STRING: %s = %s\n", setting.name.c_str(), ((std::string*)setting.value)->c_str());
                break;
        }
    }
    saveSettingsToNVS();  // Сохраняем изменения в NVS
}

// Сформировать JSON-схему для веб-интерфейса
JsonObject ConfigurableBase::getJsonSchema() const {
    static DynamicJsonDocument doc(2048);
    doc.clear();
    JsonObject schema = doc.to<JsonObject>();
    schema["module"] = getModuleName();
    schema["label"] = getModuleLabel();
    JsonArray props = schema["settings"].to<JsonArray>();
    for (const auto& setting : settings) {
        JsonObject s = props.add<JsonObject>();
        s["name"] = setting.name;
        s["label"] = setting.label;
        switch (setting.type) {
            case SettingType::BOOL:
                s["type"] = "bool";
                s["value"] = *(bool*)setting.value;
                break;
            case SettingType::INT:
                s["type"] = "int";
                s["min"] = setting.minValue;
                s["max"] = setting.maxValue;
                s["step"] = setting.step;
                s["value"] = *(int*)setting.value;
                break;
            case SettingType::FLOAT:
                s["type"] = "float";
                s["min"] = setting.minValue;
                s["max"] = setting.maxValue;
                s["step"] = setting.step;
                s["value"] = *(float*)setting.value;
                break;
            case SettingType::STRING:
                s["type"] = "string";
                s["value"] = ((std::string*)setting.value)->c_str();
                break;
        }
    }
    return schema;
}