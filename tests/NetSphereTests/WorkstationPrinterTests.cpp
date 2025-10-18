/**
 * @file WorkstationPrinterTests.cpp
 * @brief Тесты для классов Workstation и Printer.
 */

#include <gtest/gtest.h>
#include "Workstation.h"
#include "Printer.h"
#include "NetworkExceptions.h"

 /**
  * @defgroup workstation_printer_tests Тесты рабочих станций и принтеров
  * @brief Тесты для проверки функциональности Workstation и Printer
  * @{
  */

  // Тест создания Workstation с невалидными параметрами
TEST(WorkstationTest, InvalidParameters) {
    EXPECT_THROW(
        Workstation ws("", "00:1A:2B:3C:4D:5E", "user", time(nullptr)),
        ValidationException
    );

    EXPECT_THROW(
        Workstation ws("ws01", "invalid_mac", "user", time(nullptr)),
        ValidationException
    );

    EXPECT_THROW(
        Workstation ws("ws01", "00:1A:2B:3C:4D:5F", "", time(nullptr)),
        std::invalid_argument
    );
}

// Тест создания Printer с невалидными параметрами
TEST(PrinterTest, InvalidParameters) {
    EXPECT_THROW(
        Printer printer("", "00:1A:2B:3C:4D:60"),
        ValidationException
    );

    EXPECT_THROW(
        Printer printer("printer01", "invalid_mac"),
        ValidationException
    );
}

// Тест методов Workstation
TEST(WorkstationTest, WorkstationMethods) {
    time_t testTime = time(nullptr);
    Workstation ws("ws01", "00:1A:2B:3C:4D:61", "test_user", testTime);

    EXPECT_EQ(ws.getId(), "ws01");
    EXPECT_EQ(ws.getMacAddress(), "00:1A:2B:3C:4D:61");
    EXPECT_EQ(ws.getUserId(), "test_user");
    EXPECT_EQ(ws.getLastPowerOnTime(), testTime);
    EXPECT_EQ(ws.getType(), "Workstation");
}

// Тест методов Printer
TEST(PrinterTest, PrinterMethods) {
    Printer printer("printer01", "00:1A:2B:3C:4D:62");

    EXPECT_EQ(printer.getId(), "printer01");
    EXPECT_EQ(printer.getMacAddress(), "00:1A:2B:3C:4D:62");
    EXPECT_EQ(printer.getType(), "Printer");
}

// Тест обновления времени включения Workstation
TEST(WorkstationTest, UpdatePowerOnTime) {
    time_t initialTime = time(nullptr);
    Workstation ws("ws01", "00:1A:2B:3C:4D:63", "user", initialTime);

    time_t newTime = initialTime + 3600; // +1 час
    ws.updatePowerOnTime(newTime);

    EXPECT_EQ(ws.getLastPowerOnTime(), newTime);
}

/** @} */ // Конец группы workstation_printer_tests