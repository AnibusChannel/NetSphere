/**
 * @file NetworkExceptions.h
 * @brief Заголовочный файл с классами исключений для корпоративной сети.
 */

#pragma once

#include <stdexcept>
#include <string>

 /**
  * @defgroup exceptions_module Модуль исключений
  * @brief Классы исключений для обработки ошибок в корпоративной сети
  * @{
  */

  /**
   * @brief Базовый класс исключений для сетевых операций.
   */
class NetworkException : public std::runtime_error {
public:
    explicit NetworkException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Исключение для ошибок аутентификации и прав доступа.
 */
class AccessDeniedException : public NetworkException {
public:
    explicit AccessDeniedException(const std::string& message)
        : NetworkException("Ошибка доступа: " + message) {
    }
};

/**
 * @brief Исключение для ошибок валидации данных.
 */
class ValidationException : public NetworkException {
public:
    explicit ValidationException(const std::string& message)
        : NetworkException("Ошибка валидации: " + message) {
    }
};

/**
 * @brief Исключение для ошибок при операциях с устройствами.
 */
class DeviceOperationException : public NetworkException {
public:
    explicit DeviceOperationException(const std::string& message)
        : NetworkException("Ошибка операции с устройством: " + message) {
    }
};

/**
 * @brief Исключение для ошибок при операциях с доменами.
 */
class DomainOperationException : public NetworkException {
public:
    explicit DomainOperationException(const std::string& message)
        : NetworkException("Ошибка операции с доменом: " + message) {
    }
};

/** @} */ // Конец группы exceptions_module