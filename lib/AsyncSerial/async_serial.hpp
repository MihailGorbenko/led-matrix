#ifndef ASYNC_SERIAL_HPP
#define ASYNC_SERIAL_HPP

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class AsyncSerial {
private:
    SemaphoreHandle_t serialMutex; // Мьютекс для синхронизации доступа

public:
    AsyncSerial();
    ~AsyncSerial();

    // Потокобезопасный вывод строки
    void println(const char* message);

    // Потокобезопасный вывод строки с форматированием
    void printf(const char* format, ...);

    // Потокобезопасный вывод символа
    void printChar(char c);
};

#endif // ASYNC_SERIAL_HPP