/**
 * @mainpage Документация NetSphere - Система управления корпоративной сетью
 *
 * @section intro_sec Введение
 * NetSphere - комплексная система для управления устройствами и доменами в корпоративной сети компании.
 * Сеть организована в виде древовидной иерархии доменов, содержащих различные типы устройств.
 *
 * @section arch_sec Архитектура системы
 *
 * @subsection class_hierarchy Иерархия классов
 * Система построена на основе следующей иерархии классов:
 * @dot
 * digraph G {
 *     rankdir=BT;
 *     node [fontname=Helvetica, fontsize=12, shape=record, style=filled, fillcolor=lightgray];
 *     edge [fontname=Helvetica, fontsize=10];
 *     
 *     // Классы
 *     NetworkEntity [label="NetworkEntity\n«abstract»"];
 *     Device [label="Device\n«abstract»"];
 *     DataStorage [label="DataStorage"];
 *     Workstation [label="Workstation"];
 *     Printer [label="Printer"];
 *     Domain [label="Domain"];
 *     CorporateNetwork [label="CorporateNetwork"];
 *     
 *     // Наследование (обобщение)
 *     edge [arrowhead=empty];
 *     Device -> NetworkEntity;
 *     Domain -> NetworkEntity;
 *     DataStorage -> Device;
 *     Workstation -> Device;
 *     Printer -> Device;
 *     
 *     // Агрегация (CorporateNetwork содержит Domain)
 *     edge [arrowhead=odiamond, style=dashed];
 *     CorporateNetwork -> Domain [label=" содержит "];
 *     
 *     // Композиция (Domain владеет NetworkEntity)
 *     edge [arrowhead=diamond, style=solid, constraint=false];
 *     Domain -> NetworkEntity [label=" владеет "];
 * }
 * @enddot
 * 
 * @subsection uml_notation UML-нотация
 * 
 * **Типы отношений:**
 * - **Наследование (Generalization)** - стрелка с пустым треугольником
 * - **Агрегация (Aggregation)** - стрелка с пустым ромбом (слабая связь "содержит")
 * - **Композиция (Composition)** - стрелка с заполненным ромбом (сильная связь "владеет")
 * 
 * **Обозначения классов:**
 * - `«abstract»` - абстрактный класс
 * - Простые прямоугольники - конкретные классы
 * 
 * @subsection relationships_sec Отношения между классами
 * 
 * **Наследование (Generalization):**
 * - `Device` и `Domain` наследуются от абстрактного класса `NetworkEntity`
 * - `DataStorage`, `Workstation`, `Printer` наследуются от абстрактного класса `Device`
 * 
 * **Агрегация (Aggregation):**
 * - `CorporateNetwork` содержит `Domain` (отношение "has-a")
 * - Домен может существовать независимо от корпоративной сети
 * 
 * **Композиция (Composition):**
 * - `Domain` владеет коллекцией `NetworkEntity` (устройства и поддомены)
 * - При уничтожении домена уничтожаются все его сущности
 * - Отношение "часть-целое" с строгим владением
 * 
 * @subsection modules_sec Модули системы
 * Система состоит из следующих основных модулей:
 * - @ref entity_module "Базовые сущности сети"
 * - @ref device_module "Сетевые устройства"
 * - @ref storage_module "Хранилища данных"
 * - @ref workstation_module "Рабочие станции"
 * - @ref printer_module "Сетевые принтеры"
 * - @ref domain_module "Домены сети"
 * - @ref network_module "Корпоративная сеть"
 * - @ref exceptions_module "Система исключений"
 * - @ref tests_module "Модуль тестирования"
 *
 * @section features_sec Основные возможности
 *
 * @subsection device_management Управление устройствами
 * - Создание и настройка хранилищ данных, рабочих станций и принтеров
 * - Управление MAC-адресами и уникальными идентификаторами
 * - Контроль доступа через списки доверенных пользователей
 *
 * @subsection domain_management Управление доменами
 * - Создание древовидной иерархии доменов и поддоменов
 * - Назначение администраторов доменов
 * - Контроль прав доступа при операциях с доменами
 *
 * @subsection network_management Управление сетью
 * - Централизованное управление всей корпоративной сетью
 * - Рекурсивный поиск сущностей по идентификаторам
 *
 * @section usage_sec Использование системы
 *
 * @subsection basic_usage Базовое использование
 * @code{.cpp}
 * // Создание корпоративной сети
 * CorporateNetwork network("super_admin");
 *
 * // Добавление устройств в корневой домен
 * auto storage = std::make_shared<DataStorage>("main_storage", "00:1A:2B:3C:4D:5E", 5000.0);
 * network.addEntityToDomain("", storage, "super_admin");
 *
 * // Создание поддомена
 * auto devDomain = std::make_shared<Domain>("development", "dev_lead");
 * network.addEntityToDomain("", devDomain, "super_admin");
 *
 * // Вывод информации о сети
 * network.printNetworkInfo();
 * @endcode
 *
 * @subsection error_handling Обработка ошибок
 * Система использует исключения для обработки ошибок:
 * @code{.cpp}
 * try {
 *     network.addEntityToDomain("nonexistent", storage, "user");
 * } catch (const AccessDeniedException& e) {
 *     std::cout << "Ошибка доступа: " << e.what() << std::endl;
 * } catch (const ValidationException& e) {
 *     std::cout << "Ошибка валидации: " << e.what() << std::endl;
 * }
 * @endcode
 *
 * @section projects_sec Проекты решения
 * Решение состоит из двух проектов:
 * - **NetSphere** - основное приложение, статическая библиотека
 * - **NetSphereTests** - модуль тестирования на базе Google Test
 *
 * @section build_sec Сборка и развертывание
 *
 * @subsection requirements_sec Требования
 * - Компилятор C++17 или новее
 * - Graphviz (для генерации диаграмм в документации)
 * - Google Test (для модульного тестирования)
 *
 * @subsection build_instructions Инструкции по сборке
 * 1. Клонировать репозиторий проекта
 * 2. Настроить зависимости через vcpkg или вручную
 * 3. Собрать основное приложение и модуль тестирования
 * 4. Запустить тесты для проверки функциональности
 *
 * @section api_sec Основные API
 *
 * @subsection network_api Корпоративная сеть
 * - CorporateNetwork - управление всей сетью
 * - Поиск сущностей по идентификатору
 * - Рекурсивный обход доменов
 *
 * @subsection domain_api Домены
 * - Domain - контейнер для сущностей
 * - Управление правами доступа
 * - Иерархическая организация
 *
 * @subsection device_api Устройства
 * - DataStorage - управление хранилищами данных
 * - Workstation - рабочие станции пользователей
 * - Printer - сетевые принтеры
 *
 * @section testing_sec Тестирование
 * Система включает комплексные модульные тесты, покрывающие:
 * - Функциональность всех классов
 * - Обработку ошибок и исключительных ситуаций
 * - Граничные случаи и валидацию данных
 * - Производительность основных операций
 */