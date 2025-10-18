/**
 * @file NetworkExceptionsTests.cpp
 * @brief Тесты для классов исключений проекта NetSphere.
 */

#include <gtest/gtest.h>
#include "NetworkExceptions.h"

 /**
  * @defgroup exceptions_tests Тесты исключений
  * @brief Тесты для проверки системы исключений
  * @{
  */

  // Тест базового класса NetworkException
TEST(NetworkExceptionsTest, BaseException) {
    try {
        throw NetworkException("Тестовое сообщение");
    }
    catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Тестовое сообщение");
    }
}

// Тест AccessDeniedException
TEST(NetworkExceptionsTest, AccessDeniedException) {
    try {
        throw AccessDeniedException("недостаточно прав");
    }
    catch (const NetworkException& e) {
        EXPECT_STREQ(e.what(), "Ошибка доступа: недостаточно прав");
    }
}

// Тест ValidationException
TEST(NetworkExceptionsTest, ValidationException) {
    try {
        throw ValidationException("неверный формат");
    }
    catch (const NetworkException& e) {
        EXPECT_STREQ(e.what(), "Ошибка валидации: неверный формат");
    }
}

// Тест DeviceOperationException
TEST(NetworkExceptionsTest, DeviceOperationException) {
    try {
        throw DeviceOperationException("переполнение хранилища");
    }
    catch (const NetworkException& e) {
        EXPECT_STREQ(e.what(), "Ошибка операции с устройством: переполнение хранилища");
    }
}

// Тест DomainOperationException
TEST(NetworkExceptionsTest, DomainOperationException) {
    try {
        throw DomainOperationException("сущность не найдена");
    }
    catch (const NetworkException& e) {
        EXPECT_STREQ(e.what(), "Ошибка операции с доменом: сущность не найдена");
    }
}

/** @} */ // Конец группы exceptions_tests