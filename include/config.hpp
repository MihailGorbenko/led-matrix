#ifndef CONFIG_H
#define CONFIG_H

// Настройки пинов
#define LED_PIN 18
#define MIC_PIN 34

// Настройки матрицы
#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 9
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS 64

// Настройки аудиоанализатора
#define SAMPLES 128
#define SAMPLING_FREQUENCY 40000
#define NOISE_THRESHOLD 10

#endif // CONFIG_H