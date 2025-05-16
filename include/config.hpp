#ifndef CONFIG_H
#define CONFIG_H

// Настройки пинов
#define LED_PIN 18
#define MIC_PIN 34


#define UPDATE_INTERVAL (1000 / 50) // 50 FPS

// Настройки аудиоанализатора
#define SAMPLES 128 // Количество отсчетов для FFT 
#define SAMPLING_FREQUENCY 8000   // Частота дискретизации


#endif // CONFIG_H

