# Setting, SerializableSetting, PersistantSetting — Классы для описания и хранения параметров (ESP32, C++)

---

## Назначение

**Setting**, **SerializableSetting** и **PersistantSetting** — это шаблонные C++ классы, предназначенные для описания, хранения, сериализации и сохранения параметров (настроек) любого типа в проектах на ESP32.  
Они позволяют создавать настраиваемые параметры с диапазонами, шагом, значением по умолчанию, поддержкой JSON и энергонезависимой памяти (NVS).

---

## Основные возможности

- **Типобезопасное описание параметров** (`bool`, `int`, `float` и др.)
- **Диапазоны, шаг, значение по умолчанию** для числовых параметров
- **Сериализация и десериализация в/из JSON** (для интеграции с веб-интерфейсами и API)
- **Автоматическое сохранение и загрузка из NVS** (PersistantSetting)
- **Генерация JSON-схемы для UI**
- **Лёгкая интеграция с системой конфигурирования (ConfigurableBase)**

---

## Интерфейс классов

```cpp
// Базовый шаблон для хранения значения и метаданных
template<typename T>
class Setting {
public:
    Setting(const char* name, const char* label, T* value,
            T defaultValue, T minValue, T maxValue, T step);

    void reset();                // Сброс к значению по умолчанию
    bool setValue(T newValue);   // Установить значение с проверкой диапазона
    T getValue() const;          // Получить текущее значение
    // ...
};

// Добавляет методы сериализации в JSON
template<typename T>
class SerializableSetting : public Setting<T> {
public:
    void toJSON(JsonObject& obj) const;           // Сериализация всех полей в JSON
    void fromJSON(const JsonObject& obj);         // Десериализация из JSON
    JsonVariant toJsonValue() const;              // Сериализация только значения
    bool fromJsonValue(JsonVariant val);          // Десериализация только значения
    void getJsonSchema(JsonObject& obj) const;    // Генерация схемы для UI/документации
    // ...
};

// Добавляет работу с NVS и реализует ISetting
template<typename T>
class PersistantSetting : public SerializableSetting<T>, public ISetting {
public:
    PersistantSetting(const char* name, const char* label, T* value,
                      const char* nvsNamespace, T defaultValue, T minValue, T maxValue, T step);

    bool saveToNVS() const;              // Сохранить значение в NVS
    bool loadFromNVS();                  // Загрузить значение из NVS
    bool resetToDefaultAndSave();        // Сбросить к умолчанию и сохранить в NVS
    // ...
};