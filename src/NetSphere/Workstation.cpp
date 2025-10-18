/**
 * @file Workstation.cpp
 * @brief Реализация класса Workstation - рабочей станции пользователя.
 */

#pragma warning(disable : 4996)

#include "Workstation.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Workstation::Workstation(const std::string& id, const std::string& mac,
    const std::string& user, time_t powerOnTime)
    : Device(id, mac), userId(user), lastPowerOnTime(powerOnTime) {
    validateUserId(user);
}

const std::string& Workstation::getUserId() const {
    return userId;
}

time_t Workstation::getLastPowerOnTime() const {
    return lastPowerOnTime;
}

void Workstation::updatePowerOnTime(time_t newTime) {
    lastPowerOnTime = newTime;
}

void Workstation::printInfo() const {
    std::cout << "Рабочая станция: " << id << "\n";
    std::cout << "MAC: " << macAddress << "\n";
    std::cout << "Пользователь: " << userId << "\n";

    // Конвертируем время в читаемый формат
    std::tm* timeinfo = std::localtime(&lastPowerOnTime);
    std::cout << "Последнее включение: " << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << "\n";
}

std::string Workstation::getType() const {
    return "Workstation";
}