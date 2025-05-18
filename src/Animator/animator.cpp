#include <vector>
#include "../Animation/Animation.hpp"
#include "../LedMatrix/led_matrix.hpp"
#include "../AudioAnalyzer/audio_analyzer.hpp"
#include "animator.hpp"

Animator::Animator(LedMatrix* m, AudioAnalyzer* a)
    : ConfigurableBase("Animator", "Менеджер анимаций"),
      matrix(m), audio(a),
      currentAnimType(AnimationType::ColorAmplitude),
      currentAnimTypeSetting("currentAnimationType", "Тип текущей анимации", reinterpret_cast<int*>(&currentAnimType), "animator", static_cast<int>(AnimationType::ColorAmplitude))
{
    registerSetting(&currentAnimTypeSetting);
    Serial.println("[Animator] Конструктор вызван");
}

Animator::~Animator() {
    if (animTask) vTaskDelete(animTask);
    for (auto* anim : animations) delete anim;
    Serial.println("[Animator] Деструктор вызван");
}

void Animator::addAnimation(Animation* anim) {
    animations.push_back(anim);
    Serial.printf("[Animator] Добавлена анимация: %s\n", anim->getModuleName());
}

bool Animator::startAnimation(AnimationType type) {
    for (auto* anim : animations) {
        if (anim->getType() == type) {
            current = anim;
            currentAnimType = type;
            saveConfig();
            Serial.printf("[Animator] Запущена анимация: %s\n", anim->getModuleName());
            return true;
        }
    }
    Serial.println("[Animator] Не удалось найти анимацию по типу!");
    return false;
}

void Animator::animTaskFunc(void* param) {
    Animator* self = static_cast<Animator*>(param);
    Serial.println("[Animator] Задача анимации запущена");
    while (true) {
        if (self->current) {
            self->current->render(*self->matrix, self->current->needsAudio() ? self->audio : nullptr);
        }
        vTaskDelay(UPDATE_INTERVAL / portTICK_PERIOD_MS);
    }
}

void Animator::begin() {
    Serial.println("[Animator] begin()");
    startAnimation(currentAnimType);
    xTaskCreatePinnedToCore(animTaskFunc, "AnimTask", 4096, this, 1, &animTask, 1);
    Serial.println("[Animator] Анимация и задача запущены");
}

void Animator::getJsonSchema(JsonObject& obj) const {
    obj["moduleName"] = moduleName;
    obj["moduleLabel"] = moduleLabel;
    JsonArray arr = obj["animations"].to<JsonArray>();
    for (auto* anim : animations) {
        JsonObject animObj = arr.createNestedObject();
        anim->getJsonSchema(animObj);
    }
    obj["currentAnimationType"] = animationTypeToString(currentAnimType);
    Serial.println("[Animator] Схема JSON сформирована");
}

bool Animator::fromJSON(const JsonObject& obj) {
    if (obj.containsKey("currentAnimationType")) {
        AnimationType type = animationTypeFromString(obj["currentAnimationType"]);
        Serial.printf("[Animator] fromJSON: выбор анимации %s\n", animationTypeToString(type));
        startAnimation(type);
    }
    return true;
}