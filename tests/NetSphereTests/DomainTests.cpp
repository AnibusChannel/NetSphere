/**
 * @file DomainTests.cpp
 * @brief Тесты для класса Domain проекта NetSphere.
 */

#include <gtest/gtest.h>
#include "Domain.h"
#include "DataStorage.h"
#include "Workstation.h"
#include "Printer.h"

 /**
  * @defgroup domain_tests Тесты домена
  * @brief Тесты для проверки функциональности Domain
  * @{
  */

  // Тест конструктора и базовых методов Domain
TEST(DomainTest, ConstructorAndGetters) {
    Domain domain("test_domain", "admin");

    EXPECT_EQ(domain.getId(), "test_domain");
    EXPECT_EQ(domain.getAdminId(), "admin");
    EXPECT_EQ(domain.getEntityCount(), 0);
    EXPECT_EQ(domain.getType(), "Domain");
}

// Тест добавления и поиска сущностей
TEST(DomainTest, AddAndFindEntities) {
    Domain domain("test_domain", "admin");

    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:5E", 1000.0);
    auto workstation = std::make_shared<Workstation>("ws1", "00:1A:2B:3C:4D:5F", "user1", time(nullptr));
    auto printer = std::make_shared<Printer>("printer1", "00:1A:2B:3C:4D:60");

    EXPECT_NO_THROW(domain.addEntity(storage, "admin"));
    EXPECT_NO_THROW(domain.addEntity(workstation, "admin"));
    EXPECT_NO_THROW(domain.addEntity(printer, "admin"));

    EXPECT_EQ(domain.getEntityCount(), 3);

    auto foundStorage = domain.findEntity("storage1");
    ASSERT_NE(foundStorage, nullptr);
    EXPECT_EQ(foundStorage->getId(), "storage1");

    auto foundWorkstation = domain.findEntity("ws1");
    ASSERT_NE(foundWorkstation, nullptr);
    EXPECT_EQ(foundWorkstation->getId(), "ws1");

    auto foundPrinter = domain.findEntity("printer1");
    ASSERT_NE(foundPrinter, nullptr);
    EXPECT_EQ(foundPrinter->getId(), "printer1");

    auto notFound = domain.findEntity("nonexistent");
    EXPECT_EQ(notFound, nullptr);
}

// Тест удаления сущностей
TEST(DomainTest, RemoveEntities) {
    Domain domain("test_domain", "admin");

    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:61", 1000.0);
    domain.addEntity(storage, "admin");

    EXPECT_EQ(domain.getEntityCount(), 1);
    EXPECT_NO_THROW(domain.removeEntity("storage1", "admin"));
    EXPECT_EQ(domain.getEntityCount(), 0);
}

// Тест добавления поддоменов
TEST(DomainTest, AddSubdomains) {
    Domain mainDomain("main_domain", "admin");
    auto subDomain = std::make_shared<Domain>("sub_domain", "sub_admin");

    EXPECT_NO_THROW(mainDomain.addEntity(subDomain, "admin"));
    EXPECT_EQ(mainDomain.getEntityCount(), 1);

    auto foundSubdomain = mainDomain.findEntity("sub_domain");
    ASSERT_NE(foundSubdomain, nullptr);
    EXPECT_EQ(foundSubdomain->getId(), "sub_domain");
    EXPECT_EQ(foundSubdomain->getType(), "Domain");
}

// Тест метода getAllEntities
TEST(DomainTest, GetAllEntities) {
    Domain domain("test_domain", "admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:62", 1000.0);
    domain.addEntity(storage, "admin");

    const auto& allEntities = domain.getAllEntities();
    EXPECT_EQ(allEntities.size(), 1);
    EXPECT_NE(allEntities.find("storage1"), allEntities.end());
}

// Тест методов printInfo
TEST(DomainTest, PrintInfoMethods) {
    Domain domain("test_domain", "admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:63", 1000.0);
    domain.addEntity(storage, "admin");

    EXPECT_NO_THROW(domain.printInfo());
    EXPECT_NO_THROW(domain.printDetailedInfo());
}

/** @} */ // Конец группы domain_tests