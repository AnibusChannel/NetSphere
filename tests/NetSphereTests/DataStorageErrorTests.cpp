/**
 * @file DataStorageErrorTests.cpp
 * @brief Тесты для обработки ошибок в классе DataStorage.
 */

#include <gtest/gtest.h>
#include "DataStorage.h"
#include "NetworkExceptions.h"

 /**
  * @defgroup storage_error_tests Тесты ошибок хранилища
  * @brief Тесты для проверки обработки ошибок в DataStorage
  * @{
  */

  // Тест переполнения хранилища
TEST(DataStorageErrorTest, StorageOverflow) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:5E", 1000.0);
    storage += 500.0;

    EXPECT_THROW(
        storage += 600.0,
        DeviceOperationException
    );
}

// Тест освобождения большего объема, чем используется
TEST(DataStorageErrorTest, FreeMoreThanUsed) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:5F", 1000.0);
    storage += 300.0;

    EXPECT_THROW(
        storage -= 400.0,
        DeviceOperationException
    );
}

// Тест установки отрицательного размера
TEST(DataStorageErrorTest, NegativeSizeAssignment) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:60", 1000.0);

    EXPECT_THROW(
        storage = -100.0,
        DeviceOperationException
    );
}

// Тест установки размера больше общего объема
TEST(DataStorageErrorTest, SizeExceedsTotal) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:61", 1000.0);

    EXPECT_THROW(
        storage = 1500.0,
        DeviceOperationException
    );
}

// Тест добавления пустого пользователя
TEST(DataStorageErrorTest, AddEmptyUser) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:62", 1000.0);

    EXPECT_THROW(
        storage.addTrustedUser(""),
        ValidationException
    );
}

// Тест добавления дубликата пользователя
TEST(DataStorageErrorTest, AddDuplicateUser) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:63", 1000.0);
    storage.addTrustedUser("user1");

    EXPECT_THROW(
        storage.addTrustedUser("user1"),
        DeviceOperationException
    );
}

// Тест удаления несуществующего пользователя
TEST(DataStorageErrorTest, RemoveNonExistentUser) {
    DataStorage storage("test_storage", "00:1A:2B:3C:4D:64", 1000.0);
    storage.addTrustedUser("user1");

    EXPECT_THROW(
        storage.removeTrustedUser("user2"),
        DeviceOperationException
    );
}

// Тест создания хранилища с невалидным размером
TEST(DataStorageErrorTest, InvalidStorageSize) {
    EXPECT_THROW(
        DataStorage storage("test_storage", "00:1A:2B:3C:4D:65", -100.0),
        ValidationException
    );
}

// Тест создания хранилища со слишком большим размером
TEST(DataStorageErrorTest, TooLargeStorageSize) {
    EXPECT_THROW(
        DataStorage storage("test_storage", "00:1A:2B:3C:4D:66", 2e6), // 2 Петабайта
        ValidationException
    );
}

/** @} */ // Конец группы storage_error_tests