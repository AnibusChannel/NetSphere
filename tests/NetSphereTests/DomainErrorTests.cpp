/**
 * @file DomainErrorTests.cpp
 * @brief Тесты для обработки ошибок в классе Domain.
 */

#include <gtest/gtest.h>
#include "Domain.h"
#include "DataStorage.h"
#include "Workstation.h"
#include "NetworkExceptions.h"

 /**
  * @defgroup domain_error_tests Тесты ошибок домена
  * @brief Тесты для проверки обработки ошибок в Domain
  * @{
  */

  // Тест создания домена с пустым ID
TEST(DomainErrorTest, EmptyDomainId) {
    EXPECT_THROW(
        Domain domain("", "admin"),
        ValidationException
    );
}

// Тест создания домена с пустым администратором
TEST(DomainErrorTest, EmptyAdminId) {
    EXPECT_THROW(
        Domain domain("test_domain", ""),
        ValidationException
    );
}

// Тест добавления сущности без прав администратора
TEST(DomainErrorTest, AddEntityWithoutAdminRights) {
    Domain domain("test_domain", "admin");
    auto storage = std::make_shared<DataStorage>("storage", "00:1A:2B:3C:4D:5E", 1000.0);

    EXPECT_THROW(
        domain.addEntity(storage, "hacker"),
        AccessDeniedException
    );
}

// Тест удаления сущности без прав администратора
TEST(DomainErrorTest, RemoveEntityWithoutAdminRights) {
    Domain domain("test_domain", "admin");
    auto storage = std::make_shared<DataStorage>("storage", "00:1A:2B:3C:4D:5F", 1000.0);
    domain.addEntity(storage, "admin");

    EXPECT_THROW(
        domain.removeEntity("storage", "hacker"),
        AccessDeniedException
    );
}

// Тест добавления дубликата сущности
TEST(DomainErrorTest, AddDuplicateEntity) {
    Domain domain("test_domain", "admin");
    auto storage = std::make_shared<DataStorage>("storage", "00:1A:2B:3C:4D:60", 1000.0);
    domain.addEntity(storage, "admin");

    auto storage2 = std::make_shared<DataStorage>("storage", "00:1A:2B:3C:4D:61", 2000.0);

    EXPECT_THROW(
        domain.addEntity(storage2, "admin"),
        DomainOperationException
    );
}

// Тест удаления несуществующей сущности
TEST(DomainErrorTest, RemoveNonExistentEntity) {
    Domain domain("test_domain", "admin");

    EXPECT_THROW(
        domain.removeEntity("nonexistent", "admin"),
        DomainOperationException
    );
}

// Тест добавления пустой сущности
TEST(DomainErrorTest, AddNullEntity) {
    Domain domain("test_domain", "admin");

    EXPECT_THROW(
        domain.addEntity(nullptr, "admin"),
        ValidationException
    );
}

// Тест удаления сущности с пустым ID
TEST(DomainErrorTest, RemoveEntityWithEmptyId) {
    Domain domain("test_domain", "admin");

    EXPECT_THROW(
        domain.removeEntity("", "admin"),
        ValidationException
    );
}

// Тест поиска сущности с пустым ID
TEST(DomainErrorTest, FindEntityWithEmptyId) {
    Domain domain("test_domain", "admin");

    auto entity = domain.findEntity("");
    EXPECT_EQ(entity, nullptr);
}

/** @} */ // Конец группы domain_error_tests