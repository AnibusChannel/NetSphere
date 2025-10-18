/**
 * @file DataStorage.h
 * @brief Заголовочный файл класса DataStorage - хранилища данных в корпоративной сети.
 */

#pragma once

#include "Device.h"
#include <vector>
#include <compare>

 /**
  * @defgroup storage_module Модуль хранилищ данных
  * @brief Классы для работы с устройствами хранения данных
  * @{
  */

  /**
   * @brief Класс, представляющий хранилище данных в корпоративной сети компании NetSphere.
   */
class DataStorage : public Device {
private:
    double totalSizeMB;                     ///< Общий объём хранилища в мегабайтах
    double usedSizeMB;                      ///< Объём используемого пространства в мегабайтах
    std::vector<std::string> trustedUsers;  ///< Список доверенных пользователей с доступом к хранилищу

    /**
     * @brief Проверяет валидность размера хранилища.
     * @param[in] size Размер для проверки.
     * @throw ValidationException Если размер невалиден.
     */
    static void validateSize(double size) {
        if (size <= 0) {
            throw ValidationException("Размер хранилища должен быть положительным числом");
        }
        if (size > 1e6) { // 1 Петабайт
            throw ValidationException("Слишком большой размер хранилища (максимум 1 ПБ)");
        }
    }

public:
    DataStorage(const std::string& id, const std::string& mac, double totalSize);

    DataStorage& operator+=(double additionalSize);
    DataStorage& operator-=(double sizeToFree);
    DataStorage& operator=(double newSize);

    bool operator<(const DataStorage& other) const;
    bool operator==(const DataStorage& other) const;
    bool operator!=(const DataStorage& other) const;

    void addTrustedUser(const std::string& user);
    void removeTrustedUser(const std::string& user);
    bool isUserTrusted(const std::string& user) const;

    double getTotalSize() const;
    double getUsedSize() const;
    double getFreeSize() const;
    const std::vector<std::string>& getTrustedUsers() const;

    void printInfo() const override;
    std::string getType() const override;
};

/** @} */ // Конец группы storage_module