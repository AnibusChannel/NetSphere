/**
 * @file CorporateNetwork.cpp
 * @brief Реализация класса CorporateNetwork - корпоративной сети.
 */

#include "CorporateNetwork.h"
#include <iostream>

CorporateNetwork::CorporateNetwork(const std::string& rootAdminId) {
    rootDomain = std::make_shared<Domain>("root_domain", rootAdminId);
    collectAllEntities(rootDomain);
}

std::shared_ptr<Domain> CorporateNetwork::getRootDomain() const {
    return rootDomain;
}

void CorporateNetwork::addEntityToDomain(const std::string& domainId, std::shared_ptr<NetworkEntity> entity, const std::string& user) {
    // Если domainId пустой, добавляем в корневой домен
    auto targetDomain = (domainId.empty()) ? rootDomain : findDomainRecursive(rootDomain, domainId);
    if (!targetDomain) {
        throw DomainOperationException("Домен с идентификатором '" + domainId + "' не найден");
    }

    targetDomain->addEntity(entity, user);
    allEntities[entity->getId()] = entity;

    // Если добавляется домен, то нужно собрать его сущности
    if (auto newDomain = std::dynamic_pointer_cast<Domain>(entity)) {
        collectAllEntities(newDomain);
    }
}

void CorporateNetwork::removeEntity(const std::string& entityId, const std::string& user) {
    auto entity = findEntity(entityId);
    if (!entity) {
        throw DomainOperationException("Сущность с идентификатором '" + entityId + "' не найдена");
    }

    // Находим домен, содержащий эту сущность
    auto domain = findDomainContainingEntity(rootDomain, entityId);
    if (!domain) {
        throw DomainOperationException("Не удалось найти домен, содержащий сущность '" + entityId + "'");
    }

    // Удаляем из домена (метод бросает исключения при ошибках)
    domain->removeEntity(entityId, user);

    // Удаляем из общего списка
    allEntities.erase(entityId);
}

std::shared_ptr<NetworkEntity> CorporateNetwork::findEntity(const std::string& entityId) const {
    auto it = allEntities.find(entityId);
    if (it != allEntities.end()) {
        return it->second;
    }
    return nullptr;
}

void CorporateNetwork::printDomainInfo(const std::string& domainId) const {
    auto domain = (domainId.empty()) ? rootDomain : findDomainRecursive(rootDomain, domainId);
    if (!domain) {
        std::cout << "Домен не найден" << std::endl;
        return;
    }

    domain->printDetailedInfo();
}

void CorporateNetwork::printNetworkInfo() const {
    std::cout << "=== КОРПОРАТИВНАЯ СЕТЬ ===" << std::endl;
    std::cout << "Общее количество сущностей: " << allEntities.size() << std::endl;
    std::cout << "Корневой домен: " << rootDomain->getId() << " (админ: " << rootDomain->getAdminId() << ")" << std::endl;
    std::cout << "==========================" << std::endl;
}

void CorporateNetwork::collectAllEntities(std::shared_ptr<Domain> domain) {
    if (!domain) return;

    // Добавляем сам домен
    allEntities[domain->getId()] = domain;

    // Получаем все сущности домена
    const auto& entities = domain->getAllEntities();
    for (const auto& pair : entities) {
        auto entity = pair.second;
        allEntities[entity->getId()] = entity;

        // Если это домен, рекурсивно собираем его сущности
        if (auto subDomain = std::dynamic_pointer_cast<Domain>(entity)) {
            collectAllEntities(subDomain);
        }
    }
}

std::shared_ptr<Domain> CorporateNetwork::findDomainRecursive(std::shared_ptr<Domain> domain, const std::string& domainId) const {
    if (!domain) return nullptr;

    if (domain->getId() == domainId) {
        return domain;
    }

    const auto& entities = domain->getAllEntities();
    for (const auto& pair : entities) {
        if (auto subDomain = std::dynamic_pointer_cast<Domain>(pair.second)) {
            auto found = findDomainRecursive(subDomain, domainId);
            if (found) return found;
        }
    }

    return nullptr;
}

std::shared_ptr<Domain> CorporateNetwork::findDomainContainingEntity(std::shared_ptr<Domain> domain, const std::string& entityId) const {
    if (!domain) return nullptr;

    if (domain->findEntity(entityId)) {
        return domain;
    }

    const auto& entities = domain->getAllEntities();
    for (const auto& pair : entities) {
        if (auto subDomain = std::dynamic_pointer_cast<Domain>(pair.second)) {
            auto found = findDomainContainingEntity(subDomain, entityId);
            if (found) return found;
        }
    }

    return nullptr;
}