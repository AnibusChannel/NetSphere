/**
 * @file DataStorage.cpp
 * @brief Реализация класса DataStorage - хранилища данных в корпоративной сети.
 */

#include "DataStorage.h"
#include <algorithm>

 /**
  * @brief Конструктор класса DataStorage.
  * @param[in] id Уникальный строковый идентификатор хранилища.
  * @param[in] mac MAC-адрес устройства хранения данных.
  * @param[in] totalSize Общий объём хранилища в мегабайтах.
  */
DataStorage::DataStorage(const std::string& id, const std::string& mac, double totalSize)
    : id(id), macAddress(mac), totalSizeMB(totalSize), usedSizeMB(0) {
}

/**
 * @brief Оператор добавления данных к используемому объёму хранилища.
 * @param[in] additionalSize Дополнительный объём данных в мегабайтах.
 * @return Ссылка на текущий объект для поддержки цепочки вызовов.
 * @throw std::overflow_error Если добавление данных превысит общий объём хранилища.
 */
DataStorage& DataStorage::operator+=(double additionalSize) {
    if (usedSizeMB + additionalSize > totalSizeMB) {
        throw std::overflow_error("Превышение общего объема хранилища");
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
    if (usedSizeMB - sizeToFree < 0) {
        throw std::underflow_error("Нельзя освободить больше чем используется");
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
    if (newSize > totalSizeMB) {
        throw std::overflow_error("Превышение общего объема хранилища");
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
    return id < other.id;
}

/**
 * @brief Оператор проверки на равенство.
 * @param[in] other Ссылка на другой объект DataStorage для сравнения.
 * @return true если идентификаторы хранилищ равны, иначе false.
 */
bool DataStorage::operator==(const DataStorage& other) const {
    return id == other.id;
}

/**
 * @brief Оператор проверки на неравенство.
 * @param[in] other Ссылка на другой объект DataStorage для сравнения.
 * @return true если идентификаторы хранилищ не равны, иначе false.
 */
bool DataStorage::operator!=(const DataStorage& other) const {
    return id != other.id;
}

/**
 * @brief Добавляет пользователя в список доверенных.
 * @param[in] user Имя пользователя для добавления в список доверенных.
 */
void DataStorage::addTrustedUser(const std::string& user) {
    trustedUsers.push_back(user);
}

/**
 * @brief Удаляет пользователя из списка доверенных.
 * @param[in] user Имя пользователя для удаления из списка доверенных.
 */
void DataStorage::removeTrustedUser(const std::string& user) {
    trustedUsers.erase(
        std::remove(trustedUsers.begin(), trustedUsers.end(), user),
        trustedUsers.end()
    );
}

/**
 * @brief Возвращает идентификатор хранилища.
 * @return Строковый идентификатор хранилища.
 */
std::string DataStorage::getId() const {
    return id;
}

/**
 * @brief Возвращает MAC-адрес хранилища.
 * @return MAC-адрес устройства в строковом формате.
 */
std::string DataStorage::getMacAddress() const {
    return macAddress;
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
 * @brief Возвращает список доверенных пользователей.
 * @return Копия вектора с именами доверенных пользователей.
 */
std::vector<std::string> DataStorage::getTrustedUsers() const {
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

    std::cout << "Хранилище: " << id << "\n";
    std::cout << "MAC: " << macAddress << "\n";
    std::cout << "Объем: " << usedSizeMB << "/" << totalSizeMB << " MB\n";
    std::cout << "Доверенные пользователи: ";
    for (const auto& user : trustedUsers) {
        std::cout << user << " ";
    }
    std::cout << "\n";
}