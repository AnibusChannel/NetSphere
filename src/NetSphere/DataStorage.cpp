/**
 * @file DataStorage.cpp
 * @brief Реализация класса DataStorage - хранилища данных в корпоративной сети.
 */

#include "DataStorage.h"
#include <algorithm>
#include <iostream>

 /**
  * @brief Конструктор класса DataStorage.
  * @param[in] id Уникальный строковый идентификатор хранилища.
  * @param[in] mac MAC-адрес устройства хранения данных.
  * @param[in] totalSize Общий объём хранилища в мегабайтах.
  * @throw ValidationException Если параметры невалидны.
  */
DataStorage::DataStorage(const std::string& id, const std::string& mac, double totalSize)
    : Device(id, mac), totalSizeMB(totalSize), usedSizeMB(0) {
    validateSize(totalSize);
}

/**
 * @brief Оператор добавления данных к используемому объёму хранилища.
 * @param[in] additionalSize Дополнительный объём данных в мегабайтах.
 * @return Ссылка на текущий объект для поддержки цепочки вызовов.
 * @throw std::overflow_error Если добавление данных превысит общий объём хранилища.
 */
DataStorage& DataStorage::operator+=(double additionalSize) {
    if (additionalSize <= 0) {
        throw DeviceOperationException("Размер добавляемых данных должен быть положительным");
    }
    if (usedSizeMB + additionalSize > totalSizeMB) {
        throw DeviceOperationException("Превышение общего объема хранилища: невозможно добавить " +
            std::to_string(additionalSize) + " MB (свободно " +
            std::to_string(totalSizeMB - usedSizeMB) + " MB)");
    }
    usedSizeMB += additionalSize;
    return *this;
}

/**
 * @brief Оператор освобождения пространства в хранилище.
 * @param[in] sizeToFree Объём данных для освобождения в мегабайтах.
 * @return Ссылка на текущий объект для поддержки цепочки вызовов.
 * @throw std::underflow_error Если пытаемся освободить больше, чем используется.
 */
DataStorage& DataStorage::operator-=(double sizeToFree) {
    if (sizeToFree <= 0) {
        throw DeviceOperationException("Размер освобождаемых данных должен быть положительным");
    }
    if (usedSizeMB - sizeToFree < 0) {
        throw DeviceOperationException("Нельзя освободить больше чем используется: запрошено " +
            std::to_string(sizeToFree) + " MB, используется " +
            std::to_string(usedSizeMB) + " MB");
    }
    usedSizeMB -= sizeToFree;
    return *this;
}

/**
 * @brief Оператор установки нового значения используемого объёма.
 * @param[in] newSize Новое значение используемого объёма в мегабайтах.
 * @return Ссылка на текущий объект для поддержки цепочки вызовов.
 * @throw std::overflow_error Если новое значение превышает общий объём хранилища.
 */
DataStorage& DataStorage::operator=(double newSize) {
    if (newSize < 0) {
        throw DeviceOperationException("Размер используемого пространства не может быть отрицательным");
    }
    if (newSize > totalSizeMB) {
        throw DeviceOperationException("Новый размер превышает общий объем хранилища: " +
            std::to_string(newSize) + " MB > " +
            std::to_string(totalSizeMB) + " MB");
    }
    usedSizeMB = newSize;
    return *this;
}

/**
 * @brief Оператор "меньше" для сравнения хранилищ по идентификатору.
 * @param[in] other Ссылка на другой объект DataStorage для сравнения.
 * @return true если идентификатор текущего хранилища меньше идентификатора другого хранилища, иначе false.
 */
bool DataStorage::operator<(const DataStorage& other) const {
    return getId() < other.getId();
}

/**
 * @brief Оператор проверки на равенство.
 * @param[in] other Ссылка на другой объект DataStorage для сравнения.
 * @return true если идентификаторы хранилищ равны, иначе false.
 */
bool DataStorage::operator==(const DataStorage& other) const {
    return getId() == other.getId();
}

/**
 * @brief Оператор проверки на неравенство.
 * @param[in] other Ссылка на другой объект DataStorage для сравнения.
 * @return true если идентификаторы хранилищ не равны, иначе false.
 */
bool DataStorage::operator!=(const DataStorage& other) const {
    return getId() != other.getId();
}

/**
 * @brief Добавляет пользователя в список доверенных.
 * @param[in] user Имя пользователя для добавления в список доверенных.
 */
void DataStorage::addTrustedUser(const std::string& user) {
    if (user.empty()) {
        throw ValidationException("Имя пользователя не может быть пустым");
    }
    if (std::find(trustedUsers.begin(), trustedUsers.end(), user) != trustedUsers.end()) {
        throw DeviceOperationException("Пользователь " + user + " уже есть в списке доверенных");
    }
    trustedUsers.push_back(user);
}

/**
 * @brief Удаляет пользователя из списка доверенных.
 * @param[in] user Имя пользователя для удаления из списка доверенных.
 */
void DataStorage::removeTrustedUser(const std::string& user) {
    if (user.empty()) {
        throw ValidationException("Имя пользователя не может быть пустым");
    }
    auto it = std::find(trustedUsers.begin(), trustedUsers.end(), user);
    if (it == trustedUsers.end()) {
        throw DeviceOperationException("Пользователь " + user + " не найден в списке доверенных");
    }
    trustedUsers.erase(it);
}

/**
 * @brief Ищет пользователя в списке доверенных.
 * @return true если пользователь найден в списке доверенных, иначе false.
 */
bool DataStorage::isUserTrusted(const std::string& user) const {
    return std::find(trustedUsers.begin(), trustedUsers.end(), user) != trustedUsers.end();
}

/**
 * @brief Возвращает общий объём хранилища.
 * @return Общий объём хранилища в мегабайтах.
 */
double DataStorage::getTotalSize() const {
    return totalSizeMB;
}

/**
 * @brief Возвращает используемый объём хранилища.
 * @return Используемый объём хранилища в мегабайтах.
 */
double DataStorage::getUsedSize() const {
    return usedSizeMB;
}

/**
 * @brief Возвращает свободный объём хранилища.
 * @return Свободный объём хранилища в мегабайтах.
 */
double DataStorage::getFreeSize() const {
    return totalSizeMB - usedSizeMB;
}

/**
 * @brief Возвращает список доверенных пользователей.
 * @return Константная ссылка на вектор с именами доверенных пользователей.
 */
const std::vector<std::string>& DataStorage::getTrustedUsers() const {
    return trustedUsers;
}

/**
 * @brief Выводит информацию о хранилище в человекочитаемом формате.
 * @details Выводит идентификатор, MAC-адрес, информацию об объёмах
 * и список доверенных пользователей.
 */
void DataStorage::printInfo() const {

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    std::cout << "Хранилище: " << getId() << "\n";
    std::cout << "MAC: " << getMacAddress() << "\n";
    std::cout << "Объем: " << usedSizeMB << "/" << totalSizeMB << " MB\n";
    std::cout << "Доверенные пользователи: ";
    for (const auto& user : trustedUsers) {
        std::cout << user << " ";
    }
    std::cout << "\n";
}

/**
 * @brief Возвращает тип сущности.
 * @return Строка "DataStorage".
 */
std::string DataStorage::getType() const {
    return "DataStorage";
}