#ifndef CONFIG_H
#define CONFIG_H

// Настройки пинов
#define LED_PIN 18
#define MIC_PIN 34

// Настройки матрицы
#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 9
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS 50
#define UPDATE_INTERVAL (1000 / 50) // 50 FPS

// Настройки аудиоанализатора
#define SAMPLES 128 // Количество отсчетов для FFT 
#define SAMPLING_FREQUENCY 8000   // Частота дискретизации


#endif // CONFIG_H