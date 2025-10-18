/**
 * @file Printer.h
 * @brief Заголовочный файл класса Printer - сетевого принтера.
 */

#pragma once

#include "Device.h"

 /**
  * @defgroup printer_module Модуль принтеров
  * @brief Классы для работы с сетевыми принтерами
  * @{
  */

  /**
   * @brief Класс, представляющий сетевой принтер в корпоративной сети.
   */
class Printer : public Device {
public:
    /**
     * @brief Конструктор класса Printer.
     * @param[in] id Уникальный строковый идентификатор принтера.
     * @param[in] mac MAC-адрес принтера.
     */
    Printer(const std::string& id, const std::string& mac);

    void printInfo() const override;
    std::string getType() const override;
};

/** @} */ // Конец группы printer_module