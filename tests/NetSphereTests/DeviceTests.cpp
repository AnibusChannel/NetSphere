/**
 * @file DeviceTests.cpp
 * @brief Тесты для базового класса Device и его наследников.
 */

#include <gtest/gtest.h>
#include "DataStorage.h"
#include "Workstation.h"
#include "Printer.h"
#include "NetworkExceptions.h"

 /**
  * @defgroup device_tests Тесты устройств
  * @brief Тесты для проверки функциональности устройств
  * @{
  */

  // Тест создания Device с невалидным MAC-адресом
TEST(DeviceTest, InvalidMacAddress) {
    EXPECT_THROW(
        DataStorage storage("test", "invalid_mac", 1000.0),
        ValidationException
    );
}

// Тест создания Device с пустым MAC-адресом
TEST(DeviceTest, EmptyMacAddress) {
    EXPECT_THROW(
        DataStorage storage("test", "", 1000.0),
        ValidationException
    );
}

// Тест создания Device с невалидным ID
TEST(DeviceTest, InvalidDeviceId) {
    EXPECT_THROW(
        DataStorage storage("", "00:1A:2B:3C:4D:5E", 1000.0),
        ValidationException
    );
}

// Тест создания Device со слишком длинным ID
TEST(DeviceTest, TooLongDeviceId) {
    std::string longId(100, 'a');
    EXPECT_THROW(
        DataStorage storage(longId, "00:1A:2B:3C:4D:5E", 1000.0),
        ValidationException
    );
}

// Тест создания Device с недопустимыми символами в ID
TEST(DeviceTest, InvalidCharactersInDeviceId) {
    EXPECT_THROW(
        DataStorage storage("test@id", "00:1A:2B:3C:4D:5E", 1000.0),
        ValidationException
    );
}

/** @} */ // Конец группы device_tests