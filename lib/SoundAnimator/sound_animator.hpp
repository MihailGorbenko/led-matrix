#ifndef SOUND_ANIMATOR_HPP
#define SOUND_ANIMATOR_HPP

#include "led_matrix.hpp"
#include "audio_analyzer.hpp"
#include "matrix_task.hpp"
#include <Preferences.h>
#include <functional>
#include <FastLED.h>

// Перечисление типов анимаций
enum class AnimationType {
    ColorAmplitude,
    PulsingRectangle,
    StarrySky,
    Wave
};



class SoundAnimator : public MatrixTask {
public:
    SoundAnimator(LedMatrix& matrix);
    ~SoundAnimator();

    void setAnimation(AnimationType type, CRGB color = CRGB::Green);
    void update();
    void initializeAudioAnalyzer();

    void startTask() override;
    void stopTask() override;

    AudioAnalyzer& getAudioAnalyzer();

    // Параметры анимаций (сеттеры)
    void setColorAmplitudeSensitivity(float value);
    void setPulsingRectangleSensitivity(float value);
    void setStarrySkySensitivity(float value);
    void setWaveSensitivity(float value);
    void setStarrySkyMaxStars(uint8_t value);
    void setStarrySkyMinBrightness(uint8_t value);
    void setStarrySkyMaxBrightness(uint8_t value);
    void setFadeAmount(uint8_t value);
    void setWavePhaseIncrement(float value);
    void setWaveFrequency(float value);
    void setRectangleMinSize(uint8_t value);

    // Сброс и перезагрузка параметров
    void resetSettings();

    /**
     * Инициализирует настройки и загружает их из NVS.
     */
    void init();

private:
    LedMatrix& ledMatrix;
    AudioAnalyzer audioAnalyzer;

    Preferences preferences;

    unsigned long lastUpdateTime = 0;
    bool isAnimating = false;

    AnimationType currentAnimation = AnimationType::ColorAmplitude;
    CRGB currentColor = CRGB::Green;

    std::function<void()> currentRenderMethod = nullptr;

    // FreeRTOS задача
    static void animationTask(void* param);
    TaskHandle_t animationTaskHandle = nullptr;

    // Отрисовка анимаций
    void renderColorAmplitude(CRGB color);
    void renderPulsingRectangle(CRGB color);
    void renderStarrySky(CRGB color);
    void renderWave(CRGB color);

    // Загрузка и сохранение параметров
    void loadSettings();
    void saveSetting(const char* key, float value);
    void saveSetting(const char* key, uint8_t value);

    // Параметры анимаций
    float colorAmplitudeSensitivity;
    float pulsingRectangleSensitivity;
    float starrySkySensitivity;
    float waveSensitivity;
    uint8_t starrySkyMaxStars;
    uint8_t starrySkyMinBrightness;
    uint8_t starrySkyMaxBrightness;
    uint8_t fadeAmount;
    float wavePhaseIncrement;
    float waveFrequency;
    uint8_t rectangleMinSize;
};

#endif // SOUND_ANIMATOR_HPP