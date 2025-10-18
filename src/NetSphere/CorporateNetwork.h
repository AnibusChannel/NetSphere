/**
 * @file CorporateNetwork.h
 * @brief Заголовочный файл класса CorporateNetwork - корпоративной сети.
 */

#pragma once

#include "Domain.h"
#include "NetworkExceptions.h"
#include <unordered_map>
#include <memory>
#include <string>

 /**
  * @defgroup network_module Модуль корпоративной сети
  * @brief Классы для работы с корпоративной сетью
  * @{
  */

  /**
   * @brief Класс, представляющий корпоративную сеть компании.
   *
   * Содержит корневой домен и обеспечивает управление всей сетью.
   */
class CorporateNetwork {
private:
    std::shared_ptr<Domain> rootDomain; ///< Корневой домен сети
    std::unordered_map<std::string, std::shared_ptr<NetworkEntity>> allEntities; ///< Все сущности сети для быстрого поиска

    /**
     * @brief Рекурсивно собирает все сущности из домена и его поддоменов.
     * @param domain Домен, с которого начинается сбор.
     */
    void collectAllEntities(std::shared_ptr<Domain> domain);

    /**
     * @brief Рекурсивно ищет домен по идентификатору.
     * @param domain Домен, с которого начинается поиск.
     * @param domainId Идентификатор искомого домена.
     * @return Найденный домен или nullptr.
     */
    std::shared_ptr<Domain> findDomainRecursive(std::shared_ptr<Domain> domain, const std::string& domainId) const;

    /**
     * @brief Рекурсивно ищет домен, содержащий сущность с заданным идентификатором.
     * @param domain Домен, с которого начинается поиск.
     * @param entityId Идентификатор сущности.
     * @return Домен, содержащий сущность, или nullptr, если сущность не найдена.
     */
    std::shared_ptr<Domain> findDomainContainingEntity(std::shared_ptr<Domain> domain, const std::string& entityId) const;

public:
    /**
     * @brief Конструктор класса CorporateNetwork.
     * @param rootAdminId Идентификатор администратора корневого домена.
     */
    CorporateNetwork(const std::string& rootAdminId);

    /**
     * @brief Возвращает корневой домен сети.
     * @return Умный указатель на корневой домен.
     */
    std::shared_ptr<Domain> getRootDomain() const;

    /**
     * @brief Добавляет устройство в указанный домен.
     * @param domainId Идентификатор домена, в который добавляется устройство.
     * @param entity Указатель на сущность (устройство или домен) для добавления.
     * @param user Идентификатор пользователя, выполняющего операцию.
     * @throw NetworkException В случае ошибки доступа или если домен не найден.
     */
    void addEntityToDomain(const std::string& domainId, std::shared_ptr<NetworkEntity> entity, const std::string& user);

    /**
     * @brief Удаляет сущность из сети по идентификатору.
     * @param entityId Идентификатор сущности для удаления.
     * @param user Идентификатор пользователя, выполняющего операцию.
     * @throw NetworkException В случае ошибки доступа или если сущность не найдена.
     */
    void removeEntity(const std::string& entityId, const std::string& user);

    /**
     * @brief Ищет сущность в сети по идентификатору.
     * @param entityId Идентификатор сущности.
     * @return Указатель на сущность или nullptr, если сущность не найдена.
     */
    std::shared_ptr<NetworkEntity> findEntity(const std::string& entityId) const;

    /**
     * @brief Выводит информацию о домене и всех его поддоменах рекурсивно.
     * @param domainId Идентификатор домена. Если пустой, выводится корневой домен.
     */
    void printDomainInfo(const std::string& domainId = "") const;

    /**
     * @brief Выводит полную информацию о сети.
     */
    void printNetworkInfo() const;
};

/** @} */ // Конец группы network_module