# Animation

Базовый абстрактный класс для всех анимаций в проекте LED Matrix.

## Назначение

Класс `Animation` определяет интерфейс для создания различных анимаций, которые могут быть выбраны и запущены через менеджер анимаций (`Animator`). Каждая анимация может иметь собственные настройки, интеграцию с аудиосигналом и уникальный тип.

## Основные возможности

- **Наследование от ConfigurableBase:**  
  Позволяет каждой анимации иметь свои настраиваемые параметры, которые автоматически сериализуются в JSON и могут быть сохранены в энергонезависимой памяти (NVS).
- **Виртуальный метод render:**  
  Реализует основной цикл отрисовки анимации.
- **Поддержка аудиореактивных анимаций:**  
  Метод `needsAudio()` позволяет определить, требуется ли для анимации объект AudioAnalyzer.
- **Типизация анимаций:**  
  Каждый наследник реализует метод `getType()`, возвращающий уникальный тип анимации (`AnimationType`).
- **Интеграция с фронтендом:**  
  Метод `getJsonSchema()` возвращает схему настроек и метаданные для UI.
- **Установка настроек из JSON:**  
  Метод `fromJSON()` позволяет применять настройки, полученные из внешних источников (например, через API).

## Пример интерфейса класса

```cpp
class Animation : public ConfigurableBase {
public:
    Animation(const char* name, const char* label);
    virtual ~Animation();

    // Основной цикл анимации
    virtual void render(LedMatrix& matrix, AudioAnalyzer* audio) = 0;

    // Нужно ли AudioAnalyzer
    virtual bool needsAudio() const { return false; }

    // Тип анимации
    virtual AnimationType getType() const = 0;

    // JSON-схема для UI
    virtual void getJsonSchema(JsonObject& obj) const override;

    // Установка настроек из JSON
    virtual bool fromJSON(const JsonObject& obj) override;

    // Имя анимации (moduleName)
    virtual const char* getModuleName() const;
};