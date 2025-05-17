# Animator

Модуль **Animator** — это центральный менеджер анимаций для вашего LED Matrix-проекта. Он отвечает за выбор, запуск, хранение и управление всеми анимациями, а также за интеграцию с настройками, хранением состояния и взаимодействием с фронтендом через JSON API.

---

## Основные возможности

- **Регистрация и хранение анимаций**  
  Все анимации наследуются от абстрактного класса [`Animation`](../Animation/animation.hpp) и добавляются в Animator через метод `addAnimation()`.
- **Переключение анимаций по типу**  
  Для идентификации используется перечисление [`AnimationType`](../../include/AnimationType.hpp). Переключение происходит по типу, а не по имени.
- **Сохранение текущей анимации в энергонезависимой памяти (NVS)**  
  Тип текущей анимации хранится через `PersistantSetting<int>`, что позволяет восстановить состояние после перезагрузки.
- **Запуск анимации в отдельной задаче (FreeRTOS)**  
  Animator запускает отдельную задачу, в которой вызывается метод `render()` текущей анимации с заданной частотой (например, 50 FPS).
- **Интеграция с LedMatrix и AudioAnalyzer**  
  Animator управляет объектами матрицы и аудиоанализатора, передавая их в анимации по необходимости.
- **JSON API для управления и получения схемы анимаций для UI**  
  Animator предоставляет методы для получения полной схемы всех анимаций и их настроек, а также для управления текущей анимацией через JSON.

---

## Пример использования

```cpp
LedMatrix matrix;
AudioAnalyzer analyzer;
Animator animator(&matrix, &analyzer);

animator.addAnimation(new SolidColorAnimation());
animator.addAnimation(new AudioBarsAnimation());
animator.begin();
```

---

## JSON API схема

```json
{
  "moduleName": "Animator",
  "moduleLabel": "Менеджер анимаций",
  "animations": [
    {
      "moduleName": "solidColor",
      "moduleLabel": "Однотонный цвет",
      "type": "solidColor",
      "settings": [ ... ],
      "needsAudio": false
    },
    {
      "moduleName": "audioBars",
      "moduleLabel": "Аудио-столбцы",
      "type": "audioBars",
      "settings": [ ... ],
      "needsAudio": true
    }
  ],
  "currentAnimationType": "solidColor"
}
```

---

## Переключение анимации через JSON

Для смены текущей анимации отправьте:

```json
{
  "currentAnimationType": "audioBars"
}
```

---

## Расширяемость

- Для добавления новой анимации реализуйте новый класс-наследник от `Animation`, реализуйте методы `render()` и `getType()`, и добавьте его через `addAnimation()`.
- Все настройки анимаций автоматически сериализуются и доступны через JSON API.

---

**Связанные файлы:**
- [`animation.hpp`](../Animation/animation.hpp) — базовый класс анимации
- [`AnimationType.hpp`](../../include/AnimationType.hpp) — перечисление типов анимаций и функции преобразования
- [`led_matrix.hpp`](../LedMatrix/led_matrix.hpp) — управление светодиодной матрицей
- [`audio_analyzer.hpp`](../AudioAnalyzer/audio_analyzer.hpp) — аудиоанализатор

---