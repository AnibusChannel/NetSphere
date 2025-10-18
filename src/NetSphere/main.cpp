/**
 * @file main.cpp
 * @brief Главная функция приложения NetSphere.
 */

#include <iostream>
#include <locale>
#include <memory>
#include "DataStorage.h"
#include "Workstation.h"
#include "Printer.h"
#include "Domain.h"
#include "CorporateNetwork.h"

 /**
  * @brief Демонстрационная функция для тестирования класса DataStorage.
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
 * @brief Демонстрационная функция для тестирования работы доменов.
 */
void demonstrateDomainOperations() {
    std::cout << "=== Демонстрация работы с доменами ===\n" << std::endl;

    Domain mainDomain("main_domain", "super_admin");

    // Создание и добавление устройств
    auto storage = std::make_shared<DataStorage>("network_storage", "00:1A:2B:3C:4D:5F", 2000.0);
    auto workstation = std::make_shared<Workstation>("dev_workstation", "00:1A:2B:3C:4D:60",
        "developer", time(nullptr));
    auto printer = std::make_shared<Printer>("office_printer", "00:1A:2B:3C:4D:61");

    mainDomain.addEntity(storage, "super_admin");
    mainDomain.addEntity(workstation, "super_admin");
    mainDomain.addEntity(printer, "super_admin");

    std::cout << "Устройства успешно добавлены в домен" << std::endl;

    // Создание и добавление поддомена
    auto subDomain = std::make_shared<Domain>("development_domain", "dev_lead");
    auto devWorkstation = std::make_shared<Workstation>("dev_pc", "00:1A:2B:3C:4D:62",
        "coder", time(nullptr));
    subDomain->addEntity(devWorkstation, "dev_lead");
    mainDomain.addEntity(subDomain, "super_admin");

    std::cout << "Поддомен успешно добавлен" << std::endl;

    // Вывод информации
    std::cout << "\nДетальная информация о главном домене:" << std::endl;
    mainDomain.printDetailedInfo();

    // Поиск сущности
    auto foundEntity = mainDomain.findEntity("network_storage");
    if (foundEntity) {
        std::cout << "\nНайдена сущность: " << foundEntity->getId() << " (" << foundEntity->getType() << ")" << std::endl;
    }

    std::cout << "\n=== Демонстрация завершена ===\n" << std::endl;
}

/**
 * @brief Демонстрация полиморфизма и работы с разными типами устройств.
 */
void demonstratePolymorphism() {
    std::cout << "=== Демонстрация полиморфизма ===\n" << std::endl;

    // Создаем устройства разных типов, но работаем с ними через базовый класс
    std::vector<std::shared_ptr<Device>> devices;

    devices.push_back(std::make_shared<DataStorage>("backup_storage", "00:1A:2B:3C:4D:63", 5000.0));
    devices.push_back(std::make_shared<Workstation>("manager_pc", "00:1A:2B:3C:4D:64", "manager", time(nullptr)));
    devices.push_back(std::make_shared<Printer>("hr_printer", "00:1A:2B:3C:4D:65"));

    std::cout << "Информация о всех устройствах:" << std::endl;
    for (const auto& device : devices) {
        std::cout << "---" << std::endl;
        device->printInfo();
        std::cout << "Тип: " << device->getType() << std::endl;
        std::cout << "MAC: " << device->getMacAddress() << std::endl;
    }

    std::cout << "\n=== Демонстрация завершена ===\n" << std::endl;
}

/**
 * @brief Демонстрационная функция для тестирования класса CorporateNetwork.
 */
void demonstrateCorporateNetwork() {
    std::cout << "=== Демонстрация работы CorporateNetwork ===\n" << std::endl;

    CorporateNetwork network("super_admin");

    // Создание устройств
    auto storage = std::make_shared<DataStorage>("main_storage", "00:1A:2B:3C:4D:5E", 5000.0);
    auto workstation1 = std::make_shared<Workstation>("ws1", "00:1A:2B:3C:4D:5F", "user1", time(nullptr));
    auto workstation2 = std::make_shared<Workstation>("ws2", "00:1A:2B:3C:4D:60", "user2", time(nullptr));
    auto printer = std::make_shared<Printer>("printer1", "00:1A:2B:3C:4D:61");

    // Добавление устройств в корневой домен
    network.addEntityToDomain("", storage, "super_admin");
    network.addEntityToDomain("", workstation1, "super_admin");
    network.addEntityToDomain("", workstation2, "super_admin");
    network.addEntityToDomain("", printer, "super_admin");

    // Создание поддомена
    auto devDomain = std::make_shared<Domain>("development", "dev_lead");
    network.addEntityToDomain("", devDomain, "super_admin");

    // Добавление устройств в поддомен
    auto devWorkstation = std::make_shared<Workstation>("dev_ws", "00:1A:2B:3C:4D:62", "developer", time(nullptr));
    network.addEntityToDomain("development", devWorkstation, "dev_lead");

    // Вывод информации о сети
    network.printNetworkInfo();

    // Вывод информации о корневом домене
    std::cout << "\nИнформация о корневом домене:" << std::endl;
    network.printDomainInfo();

    // Поиск сущности
    auto found = network.findEntity("dev_ws");
    if (found) {
        std::cout << "\nНайдена сущность: " << found->getId() << " (" << found->getType() << ")" << std::endl;
    }

    std::cout << "\n=== Демонстрация завершена ===\n" << std::endl;
}

/**
 * @brief Главная функция приложения NetSphere.
 *
 * @return Код завершения программы.
 */
int main() {
    // Установка локали для корректного отображения кириллицы
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    std::cout << "Запуск приложения NetSphere..." << std::endl;

    try {
        // Демонстрация работы классов
        demonstrateDataStorage();
        demonstrateDomainOperations();
        demonstratePolymorphism();
        demonstrateCorporateNetwork();

    }
    catch (const std::exception& e) {
        std::cout << "Произошла ошибка: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Приложение NetSphere завершило работу." << std::endl;
    return 0;
}