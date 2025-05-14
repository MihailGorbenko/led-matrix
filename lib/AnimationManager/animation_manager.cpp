#include "animation_manager.hpp"
#include "config.hpp" // для BRIGHTNESS

AnimationManager::AnimationManager()
    : ledMatrix(),
      soundAnimator(ledMatrix),
      // staticAnimator(ledMatrix),
      currentAnimator(&soundAnimator),
      currentType(AnimatorType::Sound)
{}

void AnimationManager::begin() {
    ledMatrix.begin();
    ledMatrix.setBrightness(BRIGHTNESS);

    soundAnimator.init();
    soundAnimator.initializeAudioAnalyzer();
    soundAnimator.setAnimation(AnimationType::StarrySky, CRGB::Green);

    // staticAnimator.init(); // когда будет готов
}

void AnimationManager::start() {
    if (currentAnimator) {
        currentAnimator->startTask();
    }
}

void AnimationManager::stop() {
    if (currentAnimator) {
        currentAnimator->stopTask();
    }
}

void AnimationManager::selectAnimator(AnimatorType type) {
    if (type == currentType) return;

    stop();

    switch (type) {
        case AnimatorType::Sound:
            currentAnimator = &soundAnimator;
            break;
        case AnimatorType::Static:
            // currentAnimator = &staticAnimator;
            break;
    }

    currentType = type;
    start();
}

AnimatorType AnimationManager::getCurrentType() const {
    return currentType;
}

MatrixTask* AnimationManager::getCurrentAnimator() {
    return currentAnimator;
}

LedMatrix& AnimationManager::getLedMatrix() {
    return ledMatrix;
}

SoundAnimator& AnimationManager::getSoundAnimator() {
    return soundAnimator;
}

// StaticAnimator& AnimationManager::getStaticAnimator() {
//     return staticAnimator;
// }
