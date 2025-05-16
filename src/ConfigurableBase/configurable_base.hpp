#pragma once
#include "IConfigurable.hpp"
#include "ISetting.h"
#include <vector>
#include <ArduinoJson.h>

class ConfigurableBase : public IConfigurable {
protected:
    const char* moduleName;
    const char* moduleLabel;
    std::vector<ISetting*> settings;

public:
    ConfigurableBase(const char* name, const char* label);

    void registerSetting(ISetting* setting);

    bool loadConfig() override;
    bool saveConfig() const override;
    bool resetConfig() override;
    void toJSON(JsonObject& obj) const override;
    bool fromJSON(const JsonObject& obj) override;
    void getJsonSchema(JsonObject& obj) const override;

    
};