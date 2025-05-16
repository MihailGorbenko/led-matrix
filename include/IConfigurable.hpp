class IConfigurable {
public:
    virtual bool loadConfig() = 0;
    virtual bool saveConfig() const = 0;
    virtual bool resetConfig() = 0;
    virtual DynamicJsonDocument toJSON() const = 0;
    virtual bool fromJSON(const JsonObject& json) = 0;
    virtual ~IConfigurable() = default;
};
