/**
 * @file Domain.h
 * @brief Заголовочный файл класса Domain - домена корпоративной сети.
 */

#pragma once

#include "NetworkEntity.h"
#include "NetworkExceptions.h"
#include <unordered_map>
#include <memory>

 /**
  * @defgroup domain_module Модуль доменов
  * @brief Классы для работы с доменами сети
  * @{
  */

  /**
   * @brief Класс, представляющий домен в корпоративной сети.
   *
   * Домен может содержать устройства (хранилища, рабочие станции, принтеры)
   * и другие домены (поддомены). Имеет администратора, который управляет доменом.
   */
class Domain : public NetworkEntity {
private:
    std::string adminId;   ///< Идентификатор администратора домена
    std::unordered_map<std::string, std::shared_ptr<NetworkEntity>> entities;   ///< Хэш-таблица для хранения и быстрого поиска сущностей

    /**
     * @brief Проверяет права доступа пользователя на выполнение операций в домене.
     * @param[in] user Идентификатор пользователя.
     * @throw AccessDeniedException Если пользователь не является администратором.
     */
    void checkAdminRights(const std::string& user) const {
        if (user != adminId) {
            throw AccessDeniedException("Пользователь '" + user + "' не является администратором домена '" +
                getId() + "'. Требуются права администратора '" + adminId + "'");
        }
    }

    /**
     * @brief Проверяет валидность сущности перед добавлением.
     * @param[in] entity Сущность для проверки.
     * @throw ValidationException Если сущность невалидна.
     */
    void validateEntity(const std::shared_ptr<NetworkEntity>& entity) const {
        if (!entity) {
            throw ValidationException("Попытка добавить пустую сущность в домен");
        }
        if (entity->getId().empty()) {
            throw ValidationException("Сущность с пустым идентификатором не может быть добавлена в домен");
        }
    }

public:
    /**
     * @brief Конструктор класса Domain.
     * @param[in] id Уникальный строковый идентификатор домена.
     * @param[in] admin Идентификатор администратора домена.
     * @throw ValidationException Если параметры невалидны.
     */
    Domain(const std::string& id, const std::string& admin);

    /**
     * @brief Добавляет сущность в домен.
     * @param[in] entity Указатель на сущность для добавления.
     * @param[in] user Идентификатор пользователя, пытающегося добавить сущность.
     * @throw AccessDeniedException Если пользователь не имеет прав администратора.
     * @throw ValidationException Если сущность невалидна или уже существует.
     */
    void addEntity(std::shared_ptr<NetworkEntity> entity, const std::string& user);

    /**
     * @brief Удаляет сущность из домена по идентификатору.
     * @param[in] entityId Идентификатор сущности для удаления.
     * @param[in] user Идентификатор пользователя, пытающегося удалить сущность.
     * @throw AccessDeniedException Если пользователь не имеет прав администратора.
     * @throw DomainOperationException Если сущность не найдена.
     */
    void removeEntity(const std::string& entityId, const std::string& user);

    /**
     * @brief Ищет сущность в домене по идентификатору.
     * @param[in] entityId Идентификатор сущности.
     * @return Указатель на сущность или nullptr, если сущность не найдена.
     */
    std::shared_ptr<NetworkEntity> findEntity(const std::string& entityId) const;

    /**
     * @brief Возвращает идентификатор администратора домена.
     * @return Константная ссылка на идентификатор администратора.
     */
    const std::string& getAdminId() const;

    /**
     * @brief Возвращает количество сущностей в домене.
     * @return Количество сущностей.
     */
    size_t getEntityCount() const;

    /**
     * @brief Возвращает все сущности домена.
     * @return Константная ссылка на хэш-таблицу сущностей.
     */
    const std::unordered_map<std::string, std::shared_ptr<NetworkEntity>>& getAllEntities() const;

    void printInfo() const override;
    std::string getType() const override;

    /**
     * @brief Выводит детальную информацию о домене и всех его сущностях.
     */
    void printDetailedInfo() const;
};

/** @} */ // Конец группы domain_module