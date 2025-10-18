/**
 * @file Workstation.h
 * @brief Заголовочный файл класса Workstation - рабочей станции пользователя.
 */

#pragma once

#include "Device.h"
#include <ctime>

 /**
  * @defgroup workstation_module Модуль рабочих станций
  * @brief Классы для работы с рабочими станциями пользователей
  * @{
  */

  /**
   * @brief Класс, представляющий рабочую станцию пользователя в корпоративной сети.
   */
class Workstation : public Device {
private:
    std::string userId;         ///< Идентификатор пользователя, закрепленного за станцией
    time_t lastPowerOnTime;     ///< Время последнего включения (в секундах с эпохи Unix)

    /**
     * @brief Проверяет валидность идентификатора пользователя.
     * @param[in] user Идентификатор пользователя для проверки.
     * @throw std::invalid_argument Если идентификатор пользователя пустой.
     */
    static void validateUserId(const std::string& user) {
        if (user.empty()) {
            throw std::invalid_argument("Идентификатор пользователя не может быть пустым");
        }
    }

public:
    /**
     * @brief Конструктор класса Workstation.
     * @param[in] id Уникальный строковый идентификатор рабочей станции.
     * @param[in] mac MAC-адрес рабочей станции.
     * @param[in] user Идентификатор пользователя.
     * @param[in] powerOnTime Время последнего включения (в секундах с эпохи Unix).
     * @throw ValidationException Если передан невалидный MAC-адрес или идентификатор.
     * @throw std::invalid_argument Если идентификатор пользователя пустой.
     */
    Workstation(const std::string& id, const std::string& mac,
        const std::string& user, time_t powerOnTime);

    /**
     * @brief Возвращает идентификатор пользователя.
     * @return Константная ссылка на идентификатор пользователя.
     */
    const std::string& getUserId() const;

    /**
     * @brief Возвращает время последнего включения.
     * @return Время последнего включения в секундах с эпохи Unix.
     */
    time_t getLastPowerOnTime() const;

    /**
     * @brief Обновляет время последнего включения.
     * @param[in] newTime Новое время включения (в секундах с эпохи Unix).
     */
    void updatePowerOnTime(time_t newTime);

    void printInfo() const override;
    std::string getType() const override;
};

/** @} */ // Конец группы workstation_module