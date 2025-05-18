#pragma once
#include <cstring>

enum class AnimationType {
    ColorAmplitude,
    StarrySky,
    // Добавляйте свои типы ниже
};

inline const char* animationTypeToString(AnimationType type) {
    switch (type) {
        case AnimationType::ColorAmplitude: return "colorAmplitude";
        case AnimationType::StarrySky:  return "starrySky";
        // ...
        default: return "unknown";
    }
}

inline AnimationType animationTypeFromString(const char* str) {
    if (strcmp(str, "colorAmplitude") == 0) return AnimationType::ColorAmplitude;
    if (strcmp(str, "starrySky") == 0)  return AnimationType::StarrySky;
    // ...
    return AnimationType::ColorAmplitude; // дефолт
}