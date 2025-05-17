#pragma once
#include <cstring>

enum class AnimationType {
    ColorAmplitude,
    StarrySky,
    // Добавляйте свои типы ниже
    // Example: Rainbow, Fire, etc.
};

inline const char* animationTypeToString(AnimationType type) {
    switch (type) {
        case AnimationType::ColorAmplitude: return "colorAmplitude";
        case AnimationType::StarrySky:  return "starrySky";
        // Добавьте свои типы ниже
        default: return "unknown";
    }
}

inline AnimationType animationTypeFromString(const char* str) {
    if (strcmp(str, "colorAmplitude") == 0) return AnimationType::ColorAmplitude;
    if (strcmp(str, "starrySky") == 0)  return AnimationType::StarrySky;
    // Добавьте свои типы ниже
    return AnimationType::ColorAmplitude; // дефолт
}