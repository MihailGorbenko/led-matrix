# ConfigurableBase — Базовый класс для конфигурируемых модулей (ESP32, C++)

---

## Назначение

**ConfigurableBase** — это абстрактный C++ класс, предназначенный для создания модулей с настраиваемыми параметрами, которые можно сохранять во Flash (NVS), сериализовать/десериализовать через JSON и легко интегрировать с веб-интерфейсами или API. Используется как основа для LedMatrix и других настраиваемых компонентов.

---

## Основные возможности

- **Регистрация и хранение любых настроек** (через интерфейс `ISetting`)
- **Автоматическая загрузка/сохранение параметров в NVS**
- **Сброс к значениям по умолчанию**
- **Сериализация и десериализация всех настроек в JSON**
- **Генерация JSON-схемы для фронтенда или документации**
- **Расширяемость: легко добавлять новые параметры и модули**

---

## Интерфейс класса

```cpp
class ConfigurableBase {
public:
    ConfigurableBase(const char* name, const char* label);

    void registerSetting(ISetting* setting); // Регистрация настройки

    bool loadConfig();      // Загрузка всех настроек из NVS
    bool saveConfig() const;// Сохранение всех настроек в NVS
    bool resetConfig();     // Сброс всех настроек к значениям по умолчанию

    void toJSON(JsonObject& obj) const;      // Сериализация всех настроек в JSON
    bool fromJSON(const JsonObject& obj);    // Применение настроек из JSON

    void getJsonSchema(JsonObject& obj) const; // Генерация схемы настроек для фронтенда

protected:
    const char* moduleName;
    const char* moduleLabel;
    std::vector<ISetting*> settings;
};