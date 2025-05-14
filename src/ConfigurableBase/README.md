# ConfigurableBase

Базовый класс для создания настраиваемых модулей с поддержкой:
- хранения и загрузки настроек из NVS (Non-Volatile Storage, ESP32)
- работы с настройками через JSON (например, для веб-интерфейса)
- автоматической генерации JSON-схемы для фронтенда

---

## Основные возможности

- **Дефолтные значения**: задаются в подклассе через метод `defineDefaultSettings()`
- **Загрузка/сохранение**: значения автоматически сохраняются и загружаются из NVS
- **Работа с JSON**: поддержка применения настроек из JSON и генерация схемы для веб-интерфейса
- **Безопасность ключей**: имена ключей фильтруются и ограничиваются по длине для NVS

---

## Использование

### 1. Создайте подкласс

```cpp
class MyModule : public ConfigurableBase {
    bool enable = true;
    int volume = 10;
    std::string myName = "MyModule";
    std::string myLabel = "Мой модуль";
public:
    MyModule() {
        defineDefaultSettings();
        loadSettingsFromNVS();
    }
    void defineDefaultSettings() override {
        settings.clear();
        settings.push_back(Setting("enable", "Включить", SettingType::BOOL, &enable));
        settings.push_back(Setting("volume", "Громкость", SettingType::INT, &volume, 0, 100, 1));
    }
    const std::string& getModuleName() const override { return myName; }
    const std::string& getModuleLabel() const override { return myLabel; }
};
```

### 2. Применяйте настройки из JSON

```cpp
// Например, при получении JSON с веба:
DynamicJsonDocument doc(512);
deserializeJson(doc, jsonString);
myModule.configureJson(doc.as<JsonObject>());
```

### 3. Получайте схему для фронтенда

```cpp
JsonObject schema = myModule.getJsonSchema();
// Можно сериализовать и отправить на веб-клиент
```

### 4. Сохраняйте и загружайте настройки вручную (если нужно)

```cpp
myModule.saveSettingsToNVS();
myModule.loadSettingsFromNVS();
```

---

## Важно

- **В конструкторе подкласса обязательно вызывайте**:
  1. `defineDefaultSettings();`
  2. `loadSettingsFromNVS();`
- Все переменные, на которые указывают `void*` в `Setting`, должны жить столько же, сколько и объект класса.
- Для ESP32 требуется библиотека [`Preferences`](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html) и [`ArduinoJson`](https://arduinojson.org/).

---

## Пример JSON-схемы

```json
{
  "module": "MyModule",
  "label": "Мой модуль",
  "settings": [
    {
      "name": "enable",
      "label": "Включить",
      "type": "bool",
      "value": true
    },
    {
      "name": "volume",
      "label": "Громкость",
      "type": "int",
      "min": 0,
      "max": 100,
      "step": 1,
      "value": 42
    }
  ]
}
```

---

## Логирование

Все операции сохранения и загрузки настроек логируются через `Serial.printf` с указанием имени модуля.

---

## Лицензия

MIT (или ваша лицензия)