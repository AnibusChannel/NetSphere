/**
 * @file CorporateNetworkTests.cpp
 * @brief Тесты для класса CorporateNetwork проекта NetSphere.
 */

#include <gtest/gtest.h>
#include "CorporateNetwork.h"
#include "DataStorage.h"
#include "Workstation.h"
#include "Printer.h"
#include "Domain.h"

 /**
  * @defgroup corporate_network_tests Тесты корпоративной сети
  * @brief Тесты для проверки функциональности CorporateNetwork
  * @{
  */

  // Тест конструктора и базовых методов
TEST(CorporateNetworkTest, ConstructorAndGetters) {
    CorporateNetwork network("admin");

    auto rootDomain = network.getRootDomain();
    ASSERT_NE(rootDomain, nullptr);
    EXPECT_EQ(rootDomain->getId(), "root_domain");
    EXPECT_EQ(rootDomain->getAdminId(), "admin");
}

// Тест добавления сущности в корневой домен
TEST(CorporateNetworkTest, AddEntityToRootDomain) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:5E", 1000.0);

    EXPECT_NO_THROW(network.addEntityToDomain("", storage, "admin"));

    auto found = network.findEntity("storage1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getId(), "storage1");
}

// Тест добавления сущности в поддомен
TEST(CorporateNetworkTest, AddEntityToSubdomain) {
    CorporateNetwork network("admin");
    auto subDomain = std::make_shared<Domain>("subdomain", "sub_admin");
    network.addEntityToDomain("", subDomain, "admin");

    auto workstation = std::make_shared<Workstation>("ws1", "00:1A:2B:3C:4D:5F", "user", time(nullptr));
    EXPECT_NO_THROW(network.addEntityToDomain("subdomain", workstation, "sub_admin"));

    auto found = network.findEntity("ws1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getId(), "ws1");
}

// Тест удаления сущности
TEST(CorporateNetworkTest, RemoveEntity) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:60", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    EXPECT_NO_THROW(network.removeEntity("storage1", "admin"));

    auto found = network.findEntity("storage1");
    EXPECT_EQ(found, nullptr);
}

// Тест поиска сущности
TEST(CorporateNetworkTest, FindEntity) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:61", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    auto found = network.findEntity("storage1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getId(), "storage1");
    EXPECT_EQ(found->getType(), "DataStorage");

    auto notFound = network.findEntity("nonexistent");
    EXPECT_EQ(notFound, nullptr);
}

// Тест вывода информации о домене
TEST(CorporateNetworkTest, PrintDomainInfo) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:62", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    // Проверяем, что метод не бросает исключений
    EXPECT_NO_THROW(network.printDomainInfo());
    EXPECT_NO_THROW(network.printDomainInfo("root_domain"));
}

// Тест вывода информации о сети
TEST(CorporateNetworkTest, PrintNetworkInfo) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:63", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    EXPECT_NO_THROW(network.printNetworkInfo());
}

// Тест сбора всех сущностей
TEST(CorporateNetworkTest, CollectAllEntities) {
    CorporateNetwork network("admin");
    auto storage = std::make_shared<DataStorage>("storage1", "00:1A:2B:3C:4D:64", 1000.0);
    network.addEntityToDomain("", storage, "admin");

    auto subDomain = std::make_shared<Domain>("subdomain", "sub_admin");
    network.addEntityToDomain("", subDomain, "admin");

    auto workstation = std::make_shared<Workstation>("ws1", "00:1A:2B:3C:4D:65", "user", time(nullptr));
    network.addEntityToDomain("subdomain", workstation, "sub_admin");

    // Должны найти все три сущности: storage1, subdomain, ws1
    EXPECT_NE(network.findEntity("storage1"), nullptr);
    EXPECT_NE(network.findEntity("subdomain"), nullptr);
    EXPECT_NE(network.findEntity("ws1"), nullptr);
}

// Тест сложной иерархии доменов
TEST(CorporateNetworkTest, ComplexDomainHierarchy) {
    CorporateNetwork network("super_admin");

    // Создаем несколько уровней доменов
    auto devDomain = std::make_shared<Domain>("development", "dev_lead");
    auto qaDomain = std::make_shared<Domain>("qa", "qa_lead");

    network.addEntityToDomain("", devDomain, "super_admin");
    network.addEntityToDomain("", qaDomain, "super_admin");

    auto backendDomain = std::make_shared<Domain>("backend", "backend_lead");
    network.addEntityToDomain("development", backendDomain, "dev_lead");

    // Добавляем устройства на разных уровнях
    auto devWorkstation = std::make_shared<Workstation>("dev_ws", "00:1A:2B:3C:4D:66", "developer", time(nullptr));
    network.addEntityToDomain("development", devWorkstation, "dev_lead");

    auto backendStorage = std::make_shared<DataStorage>("backend_storage", "00:1A:2B:3C:4D:67", 2000.0);
    network.addEntityToDomain("backend", backendStorage, "backend_lead");

    auto qaWorkstation = std::make_shared<Workstation>("qa_ws", "00:1A:2B:3C:4D:68", "tester", time(nullptr));
    network.addEntityToDomain("qa", qaWorkstation, "qa_lead");

    // Проверяем, что все сущности доступны
    EXPECT_NE(network.findEntity("dev_ws"), nullptr);
    EXPECT_NE(network.findEntity("backend_storage"), nullptr);
    EXPECT_NE(network.findEntity("qa_ws"), nullptr);
    EXPECT_NE(network.findEntity("development"), nullptr);
    EXPECT_NE(network.findEntity("qa"), nullptr);
    EXPECT_NE(network.findEntity("backend"), nullptr);
}

/** @} */ // Конец группы corporate_network_tests