#include "async_serial.hpp"
#include <stdarg.h>

// Конструктор
AsyncSerial::AsyncSerial() {
    serialMutex = xSemaphoreCreateMutex(); // Создаём мьютекс
    if (serialMutex == nullptr) {
        Serial.println("[AsyncSerial] Failed to create mutex!");
    }
}

// Деструктор
AsyncSerial::~AsyncSerial() {
    if (serialMutex != nullptr) {
        vSemaphoreDelete(serialMutex); // Удаляем мьютекс
    }
}

// Потокобезопасный вывод строки
void AsyncSerial::println(const char* message) {
    if (xSemaphoreTake(serialMutex, portMAX_DELAY)) { // Захватываем мьютекс
        Serial.println(message);
        xSemaphoreGive(serialMutex); // Освобождаем мьютекс
    }
}

// Потокобезопасный вывод строки с форматированием
void AsyncSerial::printf(const char* format, ...) {
    if (xSemaphoreTake(serialMutex, portMAX_DELAY)) { // Захватываем мьютекс
        char buffer[128]; // Буфер для форматированной строки
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args); // Форматируем строку
        va_end(args);
        Serial.print(buffer); // Выводим строку в Serial
        xSemaphoreGive(serialMutex); // Освобождаем мьютекс
    }
}

// Потокобезопасный вывод символа
void AsyncSerial::printChar(char c) {
    if (xSemaphoreTake(serialMutex, portMAX_DELAY)) { // Захватываем мьютекс
        Serial.print(c);
        xSemaphoreGive(serialMutex); // Освобождаем мьютекс
    }
}