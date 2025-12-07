/**
 * @file NetSphereWrapper.h
 * @brief C API для интеграции с Python
 */

#pragma once

#ifdef NETSPHEREDLL_EXPORTS
#define NETSPHERE_API __declspec(dllexport)
#else
#define NETSPHERE_API __declspec(dllimport)
#endif

#include <string>

extern "C" {

    // Типы устройств
    enum DeviceType {
        DEVICE_DATASTORAGE = 0,
        DEVICE_WORKSTATION = 1,
        DEVICE_PRINTER = 2
    };

    // Создание и управление сетью
    NETSPHERE_API void* create_network(const char* admin_id);
    NETSPHERE_API void delete_network(void* network);
    NETSPHERE_API const char* get_network_info(void* network);

    // Управление доменами
    NETSPHERE_API int add_domain(void* network, const char* domain_id, const char* admin_id, const char* user);
    NETSPHERE_API int remove_domain(void* network, const char* domain_id, const char* user);
    NETSPHERE_API const char* get_domain_info(void* network, const char* domain_id);

    // Управление устройствами
    NETSPHERE_API int add_device(void* network, const char* domain_id, int device_type,
        const char* id, const char* mac, const char* user, ...);
    NETSPHERE_API int remove_device(void* network, const char* device_id, const char* user);
    NETSPHERE_API const char* get_device_info(void* network, const char* device_id);

    // Поиск
    NETSPHERE_API const char* find_entity(void* network, const char* entity_id);

    // Утилиты
    NETSPHERE_API void free_string(const char* str);
    NETSPHERE_API const char* get_last_error();

}