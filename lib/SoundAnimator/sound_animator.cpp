#include "sound_animator.hpp"
#include "config.hpp" // Подключаем файл конфигурации

// Конструктор
SoundAnimator::SoundAnimator(LedMatrix& matrix, AudioAnalyzer& analyzer)
    : ledMatrix(matrix), audioAnalyzer(analyzer), lastUpdateTime(0), isAnimating(false), currentAnimation(COLOR_AMPLITUDE), currentColorFunc(nullptr), animationTaskHandle(nullptr) {
    // Настраиваем общую чувствительность
    audioAnalyzer.setSensitivityReduction(5.0);

    // Настраиваем коэффициенты усиления для частот
    audioAnalyzer.setLowFreqGain(0.8);
    audioAnalyzer.setMidFreqGain(1.1);
    audioAnalyzer.setHighFreqGain(1.1);
}

// Общий метод для визуализации амплитуды
void SoundAnimator::renderAmplitude(std::function<CRGB(uint8_t)> colorFunc) {
    // Обрабатываем аудиосигнал
    audioAnalyzer.processAudio(MIC_PIN);

    // Получаем нормализованные высоты для матрицы
    uint16_t heights[MATRIX_WIDTH];
    audioAnalyzer.getNormalizedHeights(heights, MATRIX_HEIGHT);

    // Получаем доступ к массиву светодиодов
    CRGB* leds = ledMatrix.getLeds();

    // Вывод на матрицу
    ledMatrix.clear();
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = max(0, MATRIX_HEIGHT - heights[x]); y < MATRIX_HEIGHT; y++) {
            leds[ledMatrix.XY(x, y)] = colorFunc(heights[x]);
        }
    }

    // Обновляем матрицу
    ledMatrix.update();
}

// Отображение визуализации амплитуды с цветами
void SoundAnimator::startColorAmplitude() {
    isAnimating = true; // Устанавливаем флаг анимации
    currentAnimation = COLOR_AMPLITUDE; // Устанавливаем текущую анимацию
    currentColorFunc = [](uint8_t height) {
        uint8_t hue = map(height, 0, MATRIX_HEIGHT, 0, 255);
        return CHSV(hue, 255, 255); // Возвращаем цвет на основе высоты
    };
}

// Отображение визуализации амплитуды с зелёным цветом
void SoundAnimator::startGreenAmplitude() {
    isAnimating = true; // Устанавливаем флаг анимации
    currentAnimation = GREEN_AMPLITUDE; // Устанавливаем текущую анимацию
    currentColorFunc = [](uint8_t) {
        return CRGB::Green;
    };
}

// Метод для обновления визуализации
void SoundAnimator::update() {
    if (!isAnimating) {
        return; // Если анимация не активна, ничего не делаем
    }

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        renderAmplitude(currentColorFunc); // Вызываем визуализацию с текущей функцией цвета
    }
}

// Задача для обновления анимации
void SoundAnimator::animationTask(void* param) {
    SoundAnimator* animator = static_cast<SoundAnimator*>(param);

    while (true) {
        animator->update();
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL)); // Задержка в миллисекундах
    }
}

// Метод для запуска задачи
void SoundAnimator::startTask() {
    if (animationTaskHandle == nullptr) { // Проверяем, существует ли задача
        xTaskCreatePinnedToCore(
            animationTask,   // Функция задачи
            "AnimationTask", // Имя задачи
            2048,            // Размер стека
            this,            // Параметр для задачи
            1,               // Приоритет задачи
            &animationTaskHandle, // Указатель на задачу
            1                // Ядро, на котором будет выполняться задача
        );
    }
}

// Метод для остановки задачи
void SoundAnimator::stopTask() {
    if (animationTaskHandle != nullptr) {
        vTaskDelete(animationTaskHandle);
        animationTaskHandle = nullptr;
    }
}