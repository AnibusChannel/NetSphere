/**
 * @file DataStorage.h
 * @brief Заголовочный файл класса DataStorage - хранилища данных в корпоративной сети.
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <compare>
#include <stdexcept>


 /**
  * @defgroup storage_module Модуль хранилищ данных
  * @brief Классы для работы с устройствами хранения данных
  * @{
  */

 /**
  * @brief Класс, представляющий хранилище данных в корпоративной сети компании NetSphere.
  */
class DataStorage {
private:
    std::string id;                         ///< Уникальный строковый идентификатор хранилища внутри домена
    std::string macAddress;                 ///< MAC-адрес устройства хранения данных
    double totalSizeMB;                     ///< Общий объём хранилища в мегабайтах
    double usedSizeMB;                      ///< Объём используемого пространства в мегабайтах
    std::vector<std::string> trustedUsers;  ///< Список доверенных пользователей с доступом к хранилищу

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

    std::string getId() const;
    std::string getMacAddress() const;
    double getTotalSize() const;
    double getUsedSize() const;
    std::vector<std::string> getTrustedUsers() const;

    void printInfo() const;
};

/** @} */ // Конец группы storage_module