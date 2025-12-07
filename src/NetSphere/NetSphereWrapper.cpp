/**
 * @file NetSphereWrapper.cpp
 * @brief Реализация C API для Python
 */

#include "NetSphereWrapper.h"
#include "CorporateNetwork.h"
#include "DataStorage.h"
#include "Workstation.h"
#include "Printer.h"
#include "NetworkExceptions.h"
#include <sstream>
#include <cstdarg>
#include <unordered_map>

static std::string last_error;
static std::unordered_map<void*, std::shared_ptr<CorporateNetwork>> networks;

// Вспомогательная функция для обработки исключений
template<typename Func>
auto handle_exception(Func func) -> decltype(func()) {
    try {
        last_error.clear();
        return func();
    }
    catch (const std::exception& e) {
        last_error = e.what();
        return decltype(func())();
    }
    catch (...) {
        last_error = "Unknown error occurred";
        return decltype(func())();
    }
}

// Создание сети
NETSPHERE_API void* create_network(const char* admin_id) {
    return handle_exception([&]() -> void* {
        auto network = std::make_shared<CorporateNetwork>(admin_id);
        void* handle = static_cast<void*>(new std::shared_ptr<CorporateNetwork>(network));
        networks[handle] = network;
        return handle;
        });
}

// Удаление сети
NETSPHERE_API void delete_network(void* network_handle) {
    handle_exception([&]() {
        auto it = networks.find(network_handle);
        if (it != networks.end()) {
            delete static_cast<std::shared_ptr<CorporateNetwork>*>(network_handle);
            networks.erase(it);
        }
        });
}

// Информация о сети
NETSPHERE_API const char* get_network_info(void* network_handle) {
    return handle_exception([&]() -> const char* {
        auto it = networks.find(network_handle);
        if (it == networks.end()) return nullptr;

        std::stringstream ss;
        it->second->printNetworkInfo();
        // В реальной реализации нужно перехватывать вывод
        std::string result = "Network information"; // Заглушка
        return _strdup(result.c_str());
        });
}

// Добавление домена
NETSPHERE_API int add_domain(void* network_handle, const char* domain_id, const char* admin_id, const char* user) {
    return handle_exception([&]() -> int {
        auto it = networks.find(network_handle);
        if (it == networks.end()) return 0;

        auto domain = std::make_shared<Domain>(domain_id, admin_id);
        it->second->addEntityToDomain("", domain, user);
        return 1;
        });
}

// Добавление устройства
NETSPHERE_API int add_device(void* network_handle, const char* domain_id, int device_type,
    const char* id, const char* mac, const char* user, ...) {
    return handle_exception([&]() -> int {
        auto it = networks.find(network_handle);
        if (it == networks.end()) return 0;

        std::shared_ptr<Device> device;
        va_list args;
        va_start(args, user);

        switch (device_type) {
        case DEVICE_DATASTORAGE: {
            double total_size = va_arg(args, double);
            device = std::make_shared<DataStorage>(id, mac, total_size);
            break;
        }
        case DEVICE_WORKSTATION: {
            const char* user_id = va_arg(args, const char*);
            time_t power_time = va_arg(args, time_t);
            device = std::make_shared<Workstation>(id, mac, user_id, power_time);
            break;
        }
        case DEVICE_PRINTER: {
            device = std::make_shared<Printer>(id, mac);
            break;
        }
        default:
            va_end(args);
            return 0;
        }

        va_end(args);
        it->second->addEntityToDomain(domain_id, device, user);
        return 1;
        });
}

// Удаление устройства
NETSPHERE_API int remove_device(void* network_handle, const char* device_id, const char* user) {
    return handle_exception([&]() -> int {
        auto it = networks.find(network_handle);
        if (it == networks.end()) return 0;

        it->second->removeEntity(device_id, user);
        return 1;
        });
}

// Информация об устройстве
NETSPHERE_API const char* get_device_info(void* network_handle, const char* device_id) {
    return handle_exception([&]() -> const char* {
        auto it = networks.find(network_handle);
        if (it == networks.end()) return nullptr;

        auto entity = it->second->findEntity(device_id);
        if (!entity) return nullptr;

        std::stringstream ss;
        // В реальной реализации нужно получить информацию об устройстве
        std::string result = "Device: " + std::string(device_id);
        return _strdup(result.c_str());
        });
}

// Поиск сущности
NETSPHERE_API const char* find_entity(void* network_handle, const char* entity_id) {
    return handle_exception([&]() -> const char* {
        auto it = networks.find(network_handle);
        if (it == networks.end()) return nullptr;

        auto entity = it->second->findEntity(entity_id);
        if (!entity) return nullptr;

        std::string result = "Found: " + std::string(entity_id) + " (" + entity->getType() + ")";
        return _strdup(result.c_str());
        });
}

// Освобождение строк
NETSPHERE_API void free_string(const char* str) {
    if (str) {
        free(const_cast<char*>(str));
    }
}

// Получение последней ошибки
NETSPHERE_API const char* get_last_error() {
    return last_error.c_str();
}