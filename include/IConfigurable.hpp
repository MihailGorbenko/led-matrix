#pragma once
#include <string>
#include <vector>
#include "Setting.hpp"
#include <ArduinoJson.h>

class IConfigurable
{
public:
    virtual ~IConfigurable() = default;

    virtual const std::vector<Setting>& getSettings() const = 0;
    // Method to get the name of the module
    virtual const std::string& getModuleName() const = 0;
    // Method to get the label of the module
    virtual const std::string& getModuleLabel() const = 0;
    // Method to configure the object with a given configuration string
    virtual void configureJson(const JsonObject& confJson) = 0;

    // Method to get the configuration as a JSON object
    virtual JsonObject getJsonSchema() const = 0;
};
