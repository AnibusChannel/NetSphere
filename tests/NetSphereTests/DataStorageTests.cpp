/**
 * @file DataStorageTests.cpp
 * @brief Тесты для класса DataStorage проекта NetSphere.
 */

#include <gtest/gtest.h>
#include "DataStorage.h"

 /**
  * @defgroup storage_tests Тесты хранилища
  * @brief Тесты для проверки функциональности DataStorage
  * @{
  */

  // Тест конструктора и базовых методов
TEST(DataStorageTest, ConstructorAndGetters) {
    DataStorage storage("test01", "00:1A:2B:3C:4D:5E", 1000.0);

    EXPECT_EQ(storage.getId(), "test01");
    EXPECT_EQ(storage.getMacAddress(), "00:1A:2B:3C:4D:5E");
    EXPECT_EQ(storage.getTotalSize(), 1000.0);
    EXPECT_EQ(storage.getUsedSize(), 0.0);
    EXPECT_EQ(storage.getType(), "DataStorage");
}

// Тест оператора +=
TEST(DataStorageTest, AdditionOperator) {
    DataStorage storage("test02", "00:1A:2B:3C:4D:5F", 1000.0);

    storage += 500.0;
    EXPECT_EQ(storage.getUsedSize(), 500.0);

    storage += 300.0;
    EXPECT_EQ(storage.getUsedSize(), 800.0);
}

// Тест оператора -=
TEST(DataStorageTest, SubtractionOperator) {
    DataStorage storage("test03", "00:1A:2B:3C:4D:60", 1000.0);
    storage += 800.0;

    storage -= 300.0;
    EXPECT_EQ(storage.getUsedSize(), 500.0);

    storage -= 200.0;
    EXPECT_EQ(storage.getUsedSize(), 300.0);
}

// Тест оператора =
TEST(DataStorageTest, AssignmentOperator) {
    DataStorage storage("test04", "00:1A:2B:3C:4D:61", 1000.0);

    storage = 750.0;
    EXPECT_EQ(storage.getUsedSize(), 750.0);
}

// Тест операторов сравнения
TEST(DataStorageTest, ComparisonOperators) {
    DataStorage storage1("alpha", "00:1A:2B:3C:4D:62", 1000.0);
    DataStorage storage2("beta", "00:1A:2B:3C:4D:63", 1000.0);
    DataStorage storage3("alpha", "00:1A:2B:3C:4D:64", 2000.0);

    EXPECT_TRUE(storage1 < storage2);
    EXPECT_FALSE(storage2 < storage1);
    EXPECT_TRUE(storage1 == storage3);
    EXPECT_FALSE(storage1 == storage2);
}

// Тест работы с доверенными пользователями
TEST(DataStorageTest, TrustedUsers) {
    DataStorage storage("test05", "00:1A:2B:3C:4D:65", 1000.0);

    storage.addTrustedUser("user1");
    storage.addTrustedUser("user2");
    storage.addTrustedUser("admin");

    const auto& users = storage.getTrustedUsers();
    EXPECT_EQ(users.size(), 3);
    EXPECT_EQ(users[0], "user1");
    EXPECT_EQ(users[1], "user2");
    EXPECT_EQ(users[2], "admin");

    // Проверка метода isUserTrusted
    EXPECT_TRUE(storage.isUserTrusted("user1"));
    EXPECT_TRUE(storage.isUserTrusted("admin"));
    EXPECT_FALSE(storage.isUserTrusted("unknown_user"));

    storage.removeTrustedUser("user2");
    const auto& updatedUsers = storage.getTrustedUsers();
    EXPECT_EQ(updatedUsers.size(), 2);
    EXPECT_EQ(updatedUsers[0], "user1");
    EXPECT_EQ(updatedUsers[1], "admin");
}

// Тест метода getFreeSize
TEST(DataStorageTest, FreeSizeCalculation) {
    DataStorage storage("test06", "00:1A:2B:3C:4D:66", 1000.0);

    EXPECT_EQ(storage.getFreeSize(), 1000.0);

    storage += 300.0;
    EXPECT_EQ(storage.getFreeSize(), 700.0);

    storage -= 100.0;
    EXPECT_EQ(storage.getFreeSize(), 800.0);
}

// Тест метода printInfo (проверяем, что он не падает с исключением)
TEST(DataStorageTest, PrintInfoNoCrash) {
    DataStorage storage("test07", "00:1A:2B:3C:4D:67", 1000.0);
    storage.addTrustedUser("test_user");
    storage += 500.0;

    EXPECT_NO_THROW(storage.printInfo());
}

/** @} */ // Конец группы storage_tests