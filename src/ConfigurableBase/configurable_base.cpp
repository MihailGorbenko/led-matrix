#include "IConfigurable.hpp"
#include "SerializableSetting.hpp"
#include <vector>

class ConfigurableBase : public IConfigurable {
protected:
    std::vector<SerializableSettingBase*> settings;

public:
    void registerSetting(SerializableSettingBase* setting) {
        settings.push_back(setting);
    }

    bool loadConfig() override {
        bool success = true;
        for (auto* s : settings) {
            if (auto* ps = dynamic_cast<PersistantSettingBase*>(s)) {
                success &= ps->loadFromNVS();
            }
        }
        return success;
    }

    bool saveConfig() const override {
        bool success = true;
        for (auto* s : settings) {
            if (auto* ps = dynamic_cast<const PersistantSettingBase*>(s)) {
                success &= ps->saveToNVS();
            }
        }
        return success;
    }

    bool resetConfig() override {
        bool success = true;
        for (auto* s : settings) {
            if (auto* ps = dynamic_cast<PersistantSettingBase*>(s)) {
                success &= ps->resetToDefaultAndSave();
            }
        }
        return success;
    }

    DynamicJsonDocument toJSON() const override {
        DynamicJsonDocument doc(1024);
        JsonObject obj = doc.to<JsonObject>();
        for (auto* s : settings) {
            obj[s->name] = s->toJsonValue();
        }
        return doc;
    }

    bool fromJSON(const JsonObject& json) override {
        bool success = true;
        for (auto* s : settings) {
            if (json.containsKey(s->name)) {
                success &= s->fromJsonValue(json[s->name]);
            }
        }
        return success;
    }
};
