/**
 * @file Domain.cpp
 * @brief Реализация класса Domain - домена корпоративной сети.
 */

#include "Domain.h"
#include <iostream>

Domain::Domain(const std::string& id, const std::string& admin)
    : NetworkEntity(id), adminId(admin) {
    if (id.empty()) {
        throw ValidationException("Идентификатор домена не может быть пустым");
    }
    if (admin.empty()) {
        throw ValidationException("Идентификатор администратора домена не может быть пустым");
    }
}

void Domain::addEntity(std::shared_ptr<NetworkEntity> entity, const std::string& user) {
    checkAdminRights(user);
    validateEntity(entity);
    
    // Проверяем, нет ли уже сущности с таким ID
    if (entities.find(entity->getId()) != entities.end()) {
        throw DomainOperationException("Сущность с идентификатором '" + entity->getId() + 
                                     "' уже существует в домене '" + getId() + "'");
    }
    
    entities[entity->getId()] = entity;
}

void Domain::removeEntity(const std::string& entityId, const std::string& user) {
    checkAdminRights(user);
    
    if (entityId.empty()) {
        throw ValidationException("Идентификатор сущности для удаления не может быть пустым");
    }
    
    auto it = entities.find(entityId);
    if (it == entities.end()) {
        throw DomainOperationException("Сущность с идентификатором '" + entityId + 
                                     "' не найдена в домене '" + getId() + "'");
    }
    
    entities.erase(it);
}

std::shared_ptr<NetworkEntity> Domain::findEntity(const std::string& entityId) const {
    if (entityId.empty()) {
        return nullptr;
    }
    
    auto it = entities.find(entityId);
    if (it != entities.end()) {
        return it->second;
    }
    return nullptr;
}

const std::string& Domain::getAdminId() const {
    return adminId;
}

size_t Domain::getEntityCount() const {
    return entities.size();
}

const std::unordered_map<std::string, std::shared_ptr<NetworkEntity>>& Domain::getAllEntities() const {
    return entities;
}

void Domain::printInfo() const {
    std::cout << "Домен: " << getId() << "\n";
    std::cout << "Администратор: " << adminId << "\n";
    std::cout << "Количество сущностей: " << entities.size() << "\n";
}

std::string Domain::getType() const {
    return "Domain";
}

void Domain::printDetailedInfo() const {
    printInfo();
    std::cout << "Содержимое домена:\n";
    for (const auto& [id, entity] : entities) {
        std::cout << "---\n";
        entity->printInfo();
    }
    std::cout << "---\n";
}