#include "configurable_base.hpp"

ConfigurableBase::ConfigurableBase(const char* name, const char* label)
    : moduleName(name), moduleLabel(label) {}

void ConfigurableBase::registerSetting(ISetting* setting) {
    settings.push_back(setting);
}

bool ConfigurableBase::loadConfig() {
    bool success = true;
    for (auto* s : settings) {
        success &= s->loadFromNVS();
    }
    return success;
}

bool ConfigurableBase::saveConfig() const {
    bool success = true;
    for (auto* s : settings) {
        success &= s->saveToNVS();
    }
    return success;
}

bool ConfigurableBase::resetConfig() {
    bool success = true;
    for (auto* s : settings) {
        success &= s->resetToDefaultAndSave();
    }
    return success;
}

void ConfigurableBase::toJSON(JsonObject& obj) const {
    for (auto* s : settings) {
        JsonObject settingObj = obj[s->getName()].to<JsonObject>();
        s->toJSON(settingObj);
    }
}

bool ConfigurableBase::fromJSON(const JsonObject& obj) {
    bool success = true;
    for (auto* s : settings) {
        if (obj[s->getName()].is<JsonObject>()) {
            success &= s->fromJSON(obj[s->getName()]);
        }
    }
    return success;
}

void ConfigurableBase::getJsonSchema(JsonObject& obj) const {
    obj["moduleName"] = moduleName;
    obj["moduleLabel"] = moduleLabel;
    JsonArray arr = obj["settings"].to<JsonArray>();
    for (auto* s : settings) {
        JsonObject schemaObj = arr.add<JsonObject>();
        s->getJsonSchema(schemaObj);
    }
}