/**
 * @file CorporateNetworkErrorTests.cpp
 * @brief Тесты для обработки ошибок в классе CorporateNetwork.
 */

#include <gtest/gtest.h>
#include "CorporateNetwork.h"
#include "DataStorage.h"
#include "Workstation.h"
#include "NetworkExceptions.h"

 /**
  * @defgroup corporate_network_error_tests Тесты ошибок корпоративной сети
  * @brief Тесты для проверки обработки ошибок в CorporateNetwork
  * @{
  */

  // Тест добавления сущности в несуществующий домен
TEST(CorporateNetworkErrorTest, AddEntityToNonExistentDomain) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:5E", 1000.0);

    EXPECT_THROW(
        network.addEntityToDomain("nonexistent", storage, "admin"),
        DomainOperationException
    );
}

// Тест добавления сущности без прав доступа
TEST(CorporateNetworkErrorTest, AddEntityWithoutAccessRights) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:5F", 1000.0);

    EXPECT_THROW(
        network.addEntityToDomain("", storage, "hacker"),
        AccessDeniedException
    );
}

// Тест удаления несуществующей сущности
TEST(CorporateNetworkErrorTest, RemoveNonExistentEntity) {
    CorporateNetwork network("admin");

    EXPECT_THROW(
        network.removeEntity("nonexistent", "admin"),
        DomainOperationException
    );
}

// Тест удаления сущности без прав доступа
TEST(CorporateNetworkErrorTest, RemoveEntityWithoutAccessRights) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:60", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    EXPECT_THROW(
        network.removeEntity("storage1", "hacker"),
        AccessDeniedException
    );
}

// Тест добавления дубликата сущности
TEST(CorporateNetworkErrorTest, AddDuplicateEntity) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:61", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    auto storage2 = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:62", 2000.0);

    EXPECT_THROW(
        network.addEntityToDomain("", storage2, "admin"),
        DomainOperationException
    );
}

// Тест вывода информации о несуществующем домене
TEST(CorporateNetworkErrorTest, PrintNonExistentDomainInfo) {
    CorporateNetwork network("admin");

    // Не должно бросать исключение, а просто вывести сообщение
    EXPECT_NO_THROW(network.printDomainInfo("nonexistent"));
}

// Тест добавления в поддомен без прав доступа к поддомену
TEST(CorporateNetworkErrorTest, AddToSubdomainWithoutAccess) {
    CorporateNetwork network("admin");
    auto subDomain = std::make_shared<Domain>("subdomain", "sub_admin");
    network.addEntityToDomain("", subDomain, "admin");

    auto workstation = std::make_shared<Workstation>("ws1", "00:1A:2B:3C:4D:63", "user", time(nullptr));

    // Пытаемся добавить в поддомен от имени пользователя без прав
    EXPECT_THROW(
        network.addEntityToDomain("subdomain", workstation, "hacker"),
        AccessDeniedException
    );
}

// Тест удаления из поддомена без прав доступа
TEST(CorporateNetworkErrorTest, RemoveFromSubdomainWithoutAccess) {
    CorporateNetwork network("admin");
    auto subDomain = std::make_shared<Domain>("subdomain", "sub_admin");
    network.addEntityToDomain("", subDomain, "admin");

    auto workstation = std::make_shared<Workstation>("ws1", "00:1A:2B:3C:4D:64", "user", time(nullptr));
    network.addEntityToDomain("subdomain", workstation, "sub_admin");

    // Пытаемся удалить из поддомена от имени пользователя без прав
    EXPECT_THROW(
        network.removeEntity("ws1", "hacker"),
        AccessDeniedException
    );
}

// Тест добавления null сущности
TEST(CorporateNetworkErrorTest, AddNullEntity) {
    CorporateNetwork network("admin");

    EXPECT_THROW(
        network.addEntityToDomain("", nullptr, "admin"),
        ValidationException
    );
}

// Тест добавления сущности с пустым ID
TEST(CorporateNetworkErrorTest, AddEntityWithEmptyId) {
    CorporateNetwork network("admin");

    // Создаем временный объект с пустым ID, который выбросит исключение при создании
    EXPECT_THROW(
        std::make_shared<DataStorage>("", "00:1A:2B:3C:4D:65", 1000.0),
        ValidationException
    );
}


/** @} */ // Конец группы corporate_network_error_tests