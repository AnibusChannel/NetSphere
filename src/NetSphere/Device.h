/**
 * @file Device.h
 * @brief Заголовочный файл базового класса Device для всех устройств сети.
 */

#pragma once

#include "NetworkEntity.h"
#include "NetworkExceptions.h"
#include <string>
#include <regex>

 /**
  * @defgroup device_module Модуль устройств сети
  * @brief Классы для работы с сетевыми устройствами
  * @{
  */

  /**
   * @brief Абстрактный базовый класс для всех устройств корпоративной сети.
   *
   * Наследует от NetworkEntity и добавляет общие свойства устройств:
   * MAC-адрес и методы для работы с ним.
   */
class Device : public NetworkEntity {
protected:
    std::string macAddress;   ///< MAC-адрес устройства

    /**
     * @brief Проверяет валидность MAC-адреса.
     * @param[in] mac MAC-адрес для проверки.
     * @return true если MAC-адрес валиден, иначе false.
     * @details Валидный MAC-адрес должен соответствовать формату:
     * XX:XX:XX:XX:XX:XX или XX-XX-XX-XX-XX-XX, где X - шестнадцатеричная цифра.
     */
    static bool isValidMacAddress(const std::string& mac) {
        if (mac.empty()) {
            return false;
        }
        std::regex macPattern("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
        return std::regex_match(mac, macPattern);
    }

    /**
     * @brief Проверяет валидность идентификатора устройства.
     * @param[in] id Идентификатор для проверки.
     * @throw ValidationException Если идентификатор невалиден.
     */
    static void validateId(const std::string& id) {
        if (id.empty()) {
            throw ValidationException("Идентификатор устройства не может быть пустым");
        }
        if (id.length() > 50) {
            throw ValidationException("Идентификатор устройства слишком длинный (максимум 50 символов)");
        }
        // Проверка на допустимые символы
        std::regex idPattern("^[a-zA-Z0-9_-]+$");
        if (!std::regex_match(id, idPattern)) {
            throw ValidationException("Идентификатор устройства содержит недопустимые символы");
        }
    }

public:
    /**
     * @brief Конструктор базового класса Device.
     * @param[in] id Уникальный строковый идентификатор устройства.
     * @param[in] mac MAC-адрес устройства.
     * @throw ValidationException Если передан невалидный MAC-адрес или идентификатор.
     */
    Device(const std::string& id, const std::string& mac)
        : NetworkEntity(id), macAddress(mac) {
        validateId(id);
        if (!isValidMacAddress(mac)) {
            throw ValidationException("Неверный формат MAC-адреса: " + mac +
                " (ожидается формат XX:XX:XX:XX:XX:XX или XX-XX-XX-XX-XX-XX)");
        }
    }

    /**
     * @brief Возвращает MAC-адрес устройства.
     * @return Константная ссылка на MAC-адрес устройства в строковом формате.
     */
    const std::string& getMacAddress() const { return macAddress; }

    /**
     * @brief Чисто виртуальная функция для получения типа устройства.
     * @return Строка, идентифицирующая тип устройства.
     */
    virtual std::string getType() const override = 0;
};

/** @} */ // Конец группы device_module