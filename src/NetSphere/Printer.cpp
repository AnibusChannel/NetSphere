/**
 * @file Printer.cpp
 * @brief Реализация класса Printer - сетевого принтера.
 */

#include "Printer.h"
#include <iostream>

Printer::Printer(const std::string& id, const std::string& mac)
    : Device(id, mac) {
}

void Printer::printInfo() const {
    std::cout << "Принтер: " << id << "\n";
    std::cout << "MAC: " << macAddress << "\n";
}

std::string Printer::getType() const {
    return "Printer";
}