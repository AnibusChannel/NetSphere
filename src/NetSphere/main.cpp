/**
 * @file main.cpp
 * @brief Главная функция приложения NetSphere.
 */

#include <iostream>
#include "DataStorage.h"

 /**
  * @brief Демонстрационная функция для тестирования класса DataStorage.
  *
  * Эта функция будет заменена на реальную логику приложения на следующих этапах разработки.
  */
void demonstrateDataStorage() {
    std::cout << "=== Демонстрация работы класса DataStorage ===\n" << std::endl;

    // Создание хранилища данных
    DataStorage storage("main_storage", "00:1A:2B:3C:4D:5E", 1000.0);

    // Добавление данных
    storage += 500.0;
    std::cout << "Добавлено 500 МБ данных" << std::endl;

    // Добавление доверенных пользователей
    storage.addTrustedUser("admin");
    storage.addTrustedUser("user1");
    std::cout << "Добавлены доверенные пользователи" << std::endl;

    // Вывод информации о хранилище
    std::cout << "\nИнформация о хранилище:" << std::endl;
    storage.printInfo();

    // Освобождение части данных
    storage -= 200.0;
    std::cout << "\nОсвобождено 200 МБ данных" << std::endl;

    // Вывод обновленной информации
    std::cout << "Обновленная информация о хранилище:" << std::endl;
    storage.printInfo();

    std::cout << "\n=== Демонстрация завершена ===\n" << std::endl;
}

/**
 * @brief Главная функция приложения NetSphere.
 *
 * На текущем этапе служит для демонстрации работы класса DataStorage.
 * В дальнейшем будет заменена на полноценную логику приложения.
 *
 * @return Код завершения программы.
 */
int main() {

    // Установка локали для корректного отображения кириллицы
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    std::cout << "Запуск приложения NetSphere..." << std::endl;

    // Демонстрация работы класса DataStorage
    demonstrateDataStorage();

    std::cout << "Приложение NetSphere завершило работу." << std::endl;
    return 0;
}