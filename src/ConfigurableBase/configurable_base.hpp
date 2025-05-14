#pragma once
#include "IConfigurable.hpp"
#include <string>
#include <vector>
#include <ArduinoJson.h>


class ConfigurableBase : public IConfigurable
{
protected:
    std::vector<Setting> settings;
    // Подкласс должен реализовать этот метод и заполнить settings дефолтами
    virtual void defineDefaultSettings() = 0;

public:
    ConfigurableBase();
    virtual ~ConfigurableBase() = default;

    const std::vector<Setting>& getSettings() const override;
    virtual const std::string& getModuleName() const override = 0;
    virtual const std::string& getModuleLabel() const override = 0;
    void configureJson(const JsonObject& confJson) override;
    JsonObject getJsonSchema() const override;

    void saveSettingsToNVS();
    void loadSettingsFromNVS();
};

