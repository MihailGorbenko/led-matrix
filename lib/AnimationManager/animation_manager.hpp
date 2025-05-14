#pragma once

#include "led_matrix.hpp"
#include "sound_animator.hpp"
// #include "static_animator.hpp" // Раскомментируй, когда будет готов

enum class AnimatorType {
    Sound,
    Static
};

class AnimationManager {
private:
    LedMatrix ledMatrix;

    SoundAnimator soundAnimator;
    // StaticAnimator staticAnimator;

    MatrixTask* currentAnimator;
    AnimatorType currentType;

public:
    AnimationManager();

    void begin();
    void start();
    void stop();

    void selectAnimator(AnimatorType type);
    AnimatorType getCurrentType() const;

    MatrixTask* getCurrentAnimator();
    LedMatrix& getLedMatrix();
    SoundAnimator& getSoundAnimator();
    // StaticAnimator& getStaticAnimator();
};
