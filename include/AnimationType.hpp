#pragma once
#include <cstring>

enum class AnimationType {
    ColorAmplitude,
    StarrySky,
    Wave,
    PulsingRectangle,
    LiveFire,
    Snow,
    Lightning, // Добавлен тип для молнии
    MatrixRain
};

inline const char* animationTypeToString(AnimationType type) {
    switch (type) {
        case AnimationType::ColorAmplitude:    return "colorAmplitude";
        case AnimationType::StarrySky:         return "starrySky";
        case AnimationType::Wave:              return "wave";
        case AnimationType::PulsingRectangle:  return "pulsingRectangle";
        case AnimationType::LiveFire:          return "liveFire";
        case AnimationType::Snow:              return "snow";
        case AnimationType::Lightning:         return "lightning";
        case AnimationType::MatrixRain:        return "matrixRain";
        default: return "unknown";
    }
}

inline AnimationType animationTypeFromString(const char* str) {
    if (strcmp(str, "colorAmplitude") == 0)   return AnimationType::ColorAmplitude;
    if (strcmp(str, "starrySky") == 0)        return AnimationType::StarrySky;
    if (strcmp(str, "wave") == 0)             return AnimationType::Wave;
    if (strcmp(str, "pulsingRectangle") == 0) return AnimationType::PulsingRectangle;
    if (strcmp(str, "liveFire") == 0)         return AnimationType::LiveFire;
    if (strcmp(str, "snow") == 0)             return AnimationType::Snow;
    if (strcmp(str, "lightning") == 0)        return AnimationType::Lightning;
    if (strcmp(str, "matrixRain") == 0)       return AnimationType::MatrixRain;
    return AnimationType::ColorAmplitude; // дефолт
}