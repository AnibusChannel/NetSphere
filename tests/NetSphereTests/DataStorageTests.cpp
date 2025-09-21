/**
 * @file DataStorageTests.cpp
 * @brief Тесты для класса DataStorage проекта NetSphere.
 */

#include <gtest/gtest.h>
#include "DataStorage.h"  // Подключаем заголовочный файл из основного проекта

 /**
  * @defgroup tests_module Модуль тестирования
  * @brief Тесты для проверки функциональности системы
  * @{
  */

 // Тест конструктора и базовых методов
TEST(DataStorageTest, ConstructorAndGetters) {
    DataStorage storage("test01", "00:1A:2B:3C:4D:5E", 1000.0);

    // Проверяем, что конструктор правильно инициализирует поля
    EXPECT_EQ(storage.getId(), "test01");
    EXPECT_EQ(storage.getMacAddress(), "00:1A:2B:3C:4D:5E");
    EXPECT_EQ(storage.getTotalSize(), 1000.0);
    EXPECT_EQ(storage.getUsedSize(), 0.0);  // По умолчанию usedSizeMB должен быть 0
}

// Тест оператора +=
TEST(DataStorageTest, AdditionOperator) {
    DataStorage storage("test02", "00:1A:2B:3C:4D:5F", 1000.0);

    // Добавляем данные
    storage += 500.0;
    EXPECT_EQ(storage.getUsedSize(), 500.0);

    // Добавляем еще данные
    storage += 300.0;
    EXPECT_EQ(storage.getUsedSize(), 800.0);

    // Проверяем, что исключение выбрасывается при переполнении
    EXPECT_THROW(storage += 300.0, std::overflow_error);
}

// Тест оператора -=
TEST(DataStorageTest, SubtractionOperator) {
    DataStorage storage("test03", "00:1A:2B:3C:4D:60", 1000.0);
    storage += 800.0;  // Сначала добавляем данные

    // Освобождаем часть данных
    storage -= 300.0;
    EXPECT_EQ(storage.getUsedSize(), 500.0);

    // Освобождаем еще данные
    storage -= 200.0;
    EXPECT_EQ(storage.getUsedSize(), 300.0);

    // Проверяем, что исключение выбрасывается при попытке освободить больше чем есть
    EXPECT_THROW(storage -= 400.0, std::underflow_error);
}

// Тест оператора =
TEST(DataStorageTest, AssignmentOperator) {
    DataStorage storage("test04", "00:1A:2B:3C:4D:61", 1000.0);

    // Устанавливаем конкретное значение
    storage = 750.0;
    EXPECT_EQ(storage.getUsedSize(), 750.0);

    // Проверяем, что исключение выбрасывается при превышении общего объема
    EXPECT_THROW(storage = 1500.0, std::overflow_error);
}

// Тест операторов сравнения
TEST(DataStorageTest, ComparisonOperators) {
    DataStorage storage1("alpha", "00:1A:2B:3C:4D:62", 1000.0);
    DataStorage storage2("beta", "00:1A:2B:3C:4D:63", 1000.0);
    DataStorage storage3("alpha", "00:1A:2B:3C:4D:64", 2000.0);  // Тот же ID, что у storage1

    // Проверяем сравнение по идентификатору
    EXPECT_TRUE(storage1 < storage2);
    EXPECT_FALSE(storage2 < storage1);
    EXPECT_TRUE(storage1 == storage3);  // Должны быть равны, т.к. одинаковые ID
    EXPECT_FALSE(storage1 == storage2);
}

// Тест работы с доверенными пользователями
TEST(DataStorageTest, TrustedUsers) {
    DataStorage storage("test05", "00:1A:2B:3C:4D:65", 1000.0);

    // Добавляем пользователей
    storage.addTrustedUser("user1");
    storage.addTrustedUser("user2");
    storage.addTrustedUser("admin");

    // Проверяем, что пользователи добавлены
    auto users = storage.getTrustedUsers();
    EXPECT_EQ(users.size(), 3);
    EXPECT_EQ(users[0], "user1");
    EXPECT_EQ(users[1], "user2");
    EXPECT_EQ(users[2], "admin");

    // Удаляем пользователя
    storage.removeTrustedUser("user2");
    users = storage.getTrustedUsers();
    EXPECT_EQ(users.size(), 2);
    EXPECT_EQ(users[0], "user1");
    EXPECT_EQ(users[1], "admin");

    // Пытаемся удалить несуществующего пользователя (не должно быть ошибки)
    EXPECT_NO_THROW(storage.removeTrustedUser("nonexistent"));
}

// Тест метода printInfo (проверяем, что он не падает с исключением)
TEST(DataStorageTest, PrintInfoNoCrash) {
    DataStorage storage("test06", "00:1A:2B:3C:4D:66", 1000.0);
    storage.addTrustedUser("test_user");
    storage += 500.0;

    // Метод printInfo не должен бросать исключений
    EXPECT_NO_THROW(storage.printInfo());
}

// Главная функция для запуска тестов
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/** @} */ // Конец группы tests_module