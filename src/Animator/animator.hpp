#pragma once

#define UPDATE_INTERVAL (1000 / 50) // 50 FPS

#include <vector>
#include "../ConfigurableBase/configurable_base.hpp"
#include "../Animation/animation.hpp"
#include "../LedMatrix/led_matrix.hpp"
#include "../AudioAnalyzer/audio_analyzer.hpp"

class Animator : public ConfigurableBase {
    std::vector<Animation*> animations;
    Animation* current = nullptr;
    LedMatrix* matrix = nullptr;
    AudioAnalyzer* audio = nullptr;
    TaskHandle_t animTask = nullptr;
    AnimationType currentAnimType = AnimationType::ColorAmplitude;
    PersistantSetting<int> currentAnimTypeSetting;

public:
    Animator(LedMatrix* m, AudioAnalyzer* a);
    ~Animator() override;

    void addAnimation(Animation* anim);
    bool startAnimation(AnimationType type);
    void begin();

    // JSON API
    void getJsonSchema(JsonObject& obj) const override;
    bool fromJSON(const JsonObject& obj) override;
   

    Animation* getCurrentAnimation() const { return current; }
    LedMatrix* getMatrix() const { return matrix; }
    AudioAnalyzer* getAudioAnalyzer() const { return audio; }
    const std::vector<Animation*>& getAnimations() const { return animations; }

private:
    static void animTaskFunc(void* param);
};