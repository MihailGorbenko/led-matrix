#pragma once
#include "../ConfigurableBase/configurable_base.hpp"
#include "../LedMatrix/led_matrix.hpp"
#include "../AudioAnalyzer/audio_analyzer.hpp"
#include "../../include/AnimationType.hpp"

class Animation : public ConfigurableBase {
public:
    Animation(const char* name, const char* label)
        : ConfigurableBase(name, label) {}

    virtual ~Animation() {}

    // Основной цикл анимации (вызывается в задаче)
    virtual void render(LedMatrix& matrix, AudioAnalyzer* audio) = 0;

    // Нужно ли AudioAnalyzer для этой анимации
    virtual bool needsAudio() const { return false; }

    // Получить тип анимации
    virtual AnimationType getType() const = 0;

    // Для фронтенда: получить схему + needsAudio + type
    virtual void getJsonSchema(JsonObject& obj) const override {
        ConfigurableBase::getJsonSchema(obj);
        obj["needsAudio"] = needsAudio();
        obj["type"] = animationTypeToString(getType());
    }

    // Установка настроек из JSON
    virtual bool fromJSON(const JsonObject& obj) override {
        // Передаём обработку базовому классу (ConfigurableBase)
        return ConfigurableBase::fromJSON(obj);
    }

    // Получить имя анимации (moduleName)
    virtual const char* getModuleName() const { return moduleName; }
};