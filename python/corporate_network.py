"""
ctypes-обертка для взаимодействия с DLL, реализующей ядро корпоративной сети.

Модуль предоставляет:
1) загрузку DLL через `ctypes` (`load_dll`);
2) модель типа устройства (`DeviceType`) и структуры данных (`DeviceInfo`);
3) обертки над handle-объектами: `NetworkEntity` и `CorporateNetwork`;
4) обработку сообщений об ошибках, пришедших из нативной части (`decode_error_string`).

Примечание по совместимости:
имена экспортируемых C-функций в ожидаемой DLL должны соответствовать тем, которые
заданы в `load_dll` (например, `network_create`, `device_create_storage` и т.д.).
При несовпадении имен слой ctypes не сможет вызвать нужные операции.
"""

import ctypes
import os
from ctypes import c_void_p, c_char_p, c_int, c_double, c_longlong, POINTER, Structure, byref, c_char, cast, addressof
from enum import IntEnum
from datetime import datetime
from typing import Optional, List, Dict, Any


class DeviceType(IntEnum):
    """Перечисление типов устройств, используемое в ctypes-API."""
    UNKNOWN = 0
    DATA_STORAGE = 1
    WORKSTATION = 2
    PRINTER = 3
    DOMAIN = 4


class DeviceInfo(Structure):
    """Структура DeviceInfo, передаваемая в/из DLL для получения информации о сущности."""
    _fields_ = [
        ("id", ctypes.c_char * 256),
        ("mac", ctypes.c_char * 256),
        ("type", c_int),
        ("data", ctypes.c_byte * 256)
    ]


def load_dll(dll_path: str = None):
    """Загружает нативную DLL и настраивает `argtypes/restype` для C API.

    Args:
        dll_path: Полный путь к DLL. Если `None`, будет выполнен поиск в типичных местах.

    Returns:
        Загруженный объект DLL (экземпляр `ctypes.CDLL`) с настроенными сигнатурами функций.

    Raises:
        FileNotFoundError: если DLL не найдена.
        OSError: если DLL не удалось загрузить.
        CorporateNetworkError: если загрузка DLL завершилась ошибкой.
    """
    if dll_path is None:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        possible_paths = [
            os.path.join(script_dir, "NetSphere.dll"),
            os.path.join(script_dir, "corporate_network.dll"),
            "NetSphere.dll",
            "corporate_network.dll"
        ]
        
        for path in possible_paths:
            if os.path.exists(path):
                dll_path = path
                break
        
        if dll_path is None:
            raise FileNotFoundError(
                f"DLL не найдена. Искал в: {', '.join(possible_paths)}"
            )
    
    if not os.path.exists(dll_path):
        raise FileNotFoundError(f"DLL не найдена: {dll_path}")

    try:
        dll = ctypes.CDLL(dll_path)
    except OSError as e:
        raise CorporateNetworkError(f"Не удалось загрузить DLL {dll_path}: {str(e)}")

    dll.network_create.argtypes = [c_char_p]
    dll.network_create.restype = c_void_p

    dll.network_destroy.argtypes = [c_void_p]
    dll.network_destroy.restype = None

    dll.device_create_domain.argtypes = [c_char_p, c_char_p]
    dll.device_create_domain.restype = c_void_p

    dll.device_create_storage.argtypes = [c_char_p, c_char_p, c_double]
    dll.device_create_storage.restype = c_void_p

    dll.device_create_workstation.argtypes = [c_char_p, c_char_p, c_char_p, c_longlong]
    dll.device_create_workstation.restype = c_void_p

    dll.device_create_printer.argtypes = [c_char_p, c_char_p]
    dll.device_create_printer.restype = c_void_p

    dll.device_destroy.argtypes = [c_void_p]
    dll.device_destroy.restype = None

    dll.network_add_device.argtypes = [c_void_p, c_char_p, c_void_p, c_char_p]
    dll.network_add_device.restype = c_int

    dll.storage_add_trusted_user.argtypes = [c_void_p, c_char_p]
    dll.storage_add_trusted_user.restype = c_int

    dll.storage_add_data.argtypes = [c_void_p, c_double]
    dll.storage_add_data.restype = c_int

    dll.storage_free_data.argtypes = [c_void_p, c_double]
    dll.storage_free_data.restype = c_int

    dll.network_remove_device.argtypes = [c_void_p, c_char_p, c_char_p]
    dll.network_remove_device.restype = c_int

    dll.device_get_info.argtypes = [c_void_p, POINTER(DeviceInfo)]
    dll.device_get_info.restype = c_int

    dll.get_last_error.argtypes = []
    dll.get_last_error.restype = POINTER(c_char)

    dll.clear_last_error.argtypes = []
    dll.clear_last_error.restype = None

    return dll


class CorporateNetworkError(Exception):
    """Ошибка уровня обертки (ctypes/Python-слой) для корпоративной сети."""
    pass


def decode_error_string(error_ptr):
    """Пытается декодировать строку ошибки, пришедшую из DLL.

    DLL может возвращать указатель на нуль-терминированный буфер байтов.
    Код выполняет попытки декодирования в несколько кодировок (с приоритетом cp1251),
    выбирая вариант, в котором присутствуют кириллические символы или адекватная читаемость.

    Args:
        error_ptr: Указатель/значение ошибки из `ctypes` (например, `POINTER(c_char)`).

    Returns:
        Строка ошибки (возможно, пустая строка, если ошибка не распознана).
    """
    if not error_ptr:
        return ""
    
    error_bytes = None
    
    try:
        if isinstance(error_ptr, bytes):
            error_bytes = error_ptr
        elif isinstance(error_ptr, str):
            try:
                error_bytes = error_ptr.encode('latin-1')
            except:
                return error_ptr
        else:
            error_bytes = None
            
            try:
                result = []
                i = 0
                max_len = 2048
                while i < max_len:
                    try:
                        byte_val = error_ptr[i]
                        if byte_val == 0:
                            break
                        if isinstance(byte_val, int) and 0 <= byte_val <= 255:
                            result.append(byte_val)
                        else:
                            break
                        i += 1
                    except (IndexError, TypeError, ValueError, AttributeError):
                        break
                if result:
                    error_bytes = bytes(result)
            except Exception:
                pass
            
            if not error_bytes:
                try:
                    void_ptr = cast(error_ptr, c_void_p)
                    if void_ptr and void_ptr.value:
                        error_bytes = ctypes.string_at(void_ptr.value)
                        if isinstance(error_bytes, str):
                            error_bytes = error_bytes.encode('latin-1')
                    else:
                        error_bytes = ctypes.string_at(error_ptr)
                        if isinstance(error_bytes, str):
                            error_bytes = error_bytes.encode('latin-1')
                except:
                    error_bytes = b""
            
            if not error_bytes:
                error_bytes = b""
    except Exception:
        return str(error_ptr) if error_ptr else ""
    
    if not error_bytes:
        return ""
    
    try:
        decoded = error_bytes.decode('cp1251')
        if any('\u0400' <= c <= '\u04FF' for c in decoded):
            return decoded
        if '\ufffd' not in decoded and len(decoded) > 0:
            return decoded
    except (UnicodeDecodeError, AttributeError, TypeError):
        pass
    
    encodings = ['windows-1251', 'utf-8', 'latin-1', 'cp866']
    
    for encoding in encodings:
        try:
            decoded = error_bytes.decode(encoding)
            if '\ufffd' not in decoded and len(decoded) > 0:
                has_cyrillic = any('\u0400' <= c <= '\u04FF' for c in decoded)
                if has_cyrillic:
                    return decoded
                has_printable = any(c.isprintable() or c.isspace() for c in decoded)
                if has_printable and decoded.count('?') < len(decoded) * 0.2:
                    return decoded
        except (UnicodeDecodeError, AttributeError, TypeError):
            continue
    
    try:
        return error_bytes.decode('cp1251', errors='replace')
    except:
        try:
            return error_bytes.decode('utf-8', errors='replace')
        except:
            return f"<Ошибка декодирования: {error_bytes.hex()[:50]}>"


class NetworkEntity:
    """Обертка над нативным handle конкретной сущности сети (устройство или домен).

    Внутри хранится `handle` (opaque pointer) и ссылка на загруженную DLL.
    Для операций используется передача handle обратно в функции DLL.
    """
    def __init__(self, handle: c_void_p, dll):
        """Создает Python-обертку над нативной сущностью.

        Args:
            handle: opaque handle (указатель), возвращенный DLL.
            dll: объект `ctypes.CDLL`, из которого выполняются вызовы.
        """
        self.handle = handle
        self.dll = dll

    def __del__(self):
        """Освобождает нативный объект через `device_destroy`, если handle не пуст."""
        if self.handle:
            self.dll.device_destroy(self.handle)

    def storage_add_data(self, size: float) -> bool:
        """Добавляет объем данных в хранилище.

        Args:
            size: Количество данных (MB), которое требуется добавить.

        Returns:
            True при успехе, False при отказе (если при этом не было детальной ошибки).

        Raises:
            CorporateNetworkError: если DLL вернула ошибку и выдала текст ошибки.
        """
        result = self.dll.storage_add_data(self.handle, c_double(size))
        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False
        return True

    def storage_free_data(self, size: float) -> bool:
        """Освобождает (уменьшает) объем данных в хранилище.

        Args:
            size: Количество данных (MB), которое нужно освободить.

        Returns:
            True при успехе, False при отказе.

        Raises:
            CorporateNetworkError: если DLL вернула ошибку и выдала текст ошибки.
        """
        result = self.dll.storage_free_data(self.handle, c_double(size))
        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False
        return True

    def storage_add_trusted_user(self, user: str) -> bool:
        """Добавляет доверенного пользователя к хранилищу.

        Args:
            user: Идентификатор пользователя, добавляемого в список доверенных.

        Returns:
            True при успехе, False при отказе.

        Raises:
            CorporateNetworkError: если DLL вернула ошибку и выдала текст ошибки.
        """
        result = self.dll.storage_add_trusted_user(self.handle, user.encode('utf-8'))
        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False
        return True

    def get_info(self) -> Dict[str, Any]:
        """Запрашивает информацию о сущности.

        Ожидает, что DLL заполнит структуру `DeviceInfo`.

        Returns:
            Словарь вида:
              - `id`: str
              - `mac`: str (может быть пустой)
              - `type`: DeviceType

            Если DLL не вернула информацию, возвращает пустой словарь `{}`.

        Raises:
            CorporateNetworkError: если DLL вернула ошибку и выдала текст ошибки.
        """
        info = DeviceInfo()

        if self.dll.device_get_info(self.handle, byref(info)):
            id_str = info.id.decode('utf-8').rstrip('\x00')
            mac_str = info.mac.decode('utf-8').rstrip('\x00') if info.mac else ''
            
            result = {
                'id': id_str,
                'mac': mac_str,
                'type': DeviceType(info.type)
            }

            if result['type'] == DeviceType.DATA_STORAGE:
                pass
            elif result['type'] == DeviceType.WORKSTATION:
                pass
            elif result['type'] == DeviceType.DOMAIN:
                pass

            return result

        error = self.dll.get_last_error()
        if error:
            error_str = decode_error_string(error)
            raise CorporateNetworkError(error_str)
        return {}


class CorporateNetwork:
    """Основная Python-обертка над сетью корпоративных доменов.

    Сеть создается через handle, возвращаемый DLL. Далее операции выполняются через вызовы DLL.
    В качестве объектов сущностей возвращаются `NetworkEntity` (opaque handle).
    """
    def __init__(self, root_admin_id: str, dll_path: str = None):
        """Создает сеть с корневым администратором.

        Args:
            root_admin_id: Идентификатор администратора корневого домена.
            dll_path: Необязательный путь к DLL.

        Raises:
            CorporateNetworkError: если DLL не удалось загрузить или сеть не удалось создать.
        """
        try:
            self.dll = load_dll(dll_path)
        except Exception as e:
            raise CorporateNetworkError(f"Ошибка загрузки DLL: {str(e)}")
        
        try:
            self.handle = self.dll.network_create(root_admin_id.encode('utf-8'))
        except Exception as e:
            raise CorporateNetworkError(f"Ошибка при создании сети: {str(e)}")

        if not self.handle:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            raise CorporateNetworkError("Не удалось создать сеть")

        self.devices = []

    def __del__(self):
        """Освобождает нативную сеть через `network_destroy`, если handle не пуст."""
        if self.handle:
            self.dll.network_destroy(self.handle)

    def create_domain(self, domain_id: str, admin_id: str) -> NetworkEntity:
        """Создает домен в нативной части и оборачивает его в `NetworkEntity`.

        Args:
            domain_id: Идентификатор домена.
            admin_id: Идентификатор администратора домена.

        Returns:
            NetworkEntity с созданным доменом.

        Raises:
            CorporateNetworkError: если DLL не вернула валидный handle.
        """
        handle = self.dll.device_create_domain(
            domain_id.encode('utf-8'),
            admin_id.encode('utf-8')
        )
        if not handle:
            error = self.dll.get_last_error()
            error_str = decode_error_string(error) if error else "Ошибка создания домена"
            raise CorporateNetworkError(error_str)

        device = NetworkEntity(handle, self.dll)
        self.devices.append(device)
        return device

    def create_storage(self, device_id: str, mac: str, total_size: float) -> NetworkEntity:
        """Создает хранилище данных в нативной части.

        Args:
            device_id: Идентификатор устройства.
            mac: MAC-адрес устройства.
            total_size: Общий объем хранилища (MB).

        Returns:
            NetworkEntity с созданным хранилищем.

        Raises:
            CorporateNetworkError: если DLL не вернула валидный handle.
        """
        handle = self.dll.device_create_storage(
            device_id.encode('utf-8'),
            mac.encode('utf-8'),
            c_double(total_size)
        )
        if not handle:
            error = self.dll.get_last_error()
            error_str = decode_error_string(error) if error else "Ошибка создания хранилища"
            raise CorporateNetworkError(error_str)

        device = NetworkEntity(handle, self.dll)
        self.devices.append(device)
        return device

    def create_workstation(self, device_id: str, mac: str, user_id: str, power_on_time: int) -> NetworkEntity:
        """Создает рабочую станцию в нативной части.

        Args:
            device_id: Идентификатор устройства.
            mac: MAC-адрес устройства.
            user_id: Идентификатор пользователя, закрепленного за станцией.
            power_on_time: Время последнего включения (обычно seconds since epoch).

        Returns:
            NetworkEntity с созданной рабочей станцией.

        Raises:
            CorporateNetworkError: если DLL не вернула валидный handle.
        """
        handle = self.dll.device_create_workstation(
            device_id.encode('utf-8'),
            mac.encode('utf-8'),
            user_id.encode('utf-8'),
            c_longlong(power_on_time)
        )
        if not handle:
            error = self.dll.get_last_error()
            error_str = decode_error_string(error) if error else "Ошибка создания рабочей станции"
            raise CorporateNetworkError(error_str)

        device = NetworkEntity(handle, self.dll)
        self.devices.append(device)
        return device

    def create_printer(self, device_id: str, mac: str) -> NetworkEntity:
        """Создает принтер в нативной части.

        Args:
            device_id: Идентификатор принтера.
            mac: MAC-адрес принтера.

        Returns:
            NetworkEntity с созданным принтером.

        Raises:
            CorporateNetworkError: если DLL не вернула валидный handle.
        """
        handle = self.dll.device_create_printer(
            device_id.encode('utf-8'),
            mac.encode('utf-8')
        )
        if not handle:
            error = self.dll.get_last_error()
            error_str = decode_error_string(error) if error else "Ошибка создания принтера"
            raise CorporateNetworkError(error_str)

        device = NetworkEntity(handle, self.dll)
        self.devices.append(device)
        return device

    def add_device_to_domain(self, domain_id: str, device: NetworkEntity, user: str) -> bool:
        """Добавляет созданную сущность в домен.

        Типичный сценарий:
        1) `create_*` создает устройство/домен и возвращает `NetworkEntity`;
        2) `add_device_to_domain` связывает сущность с нужным доменом в нативной части.

        Args:
            domain_id: Идентификатор домена (пустая строка может означать корневой домен, в зависимости от DLL).
            device: Сущность, которая добавляется.
            user: Идентификатор пользователя, выполняющего операцию (для проверки прав).

        Returns:
            True при успехе, False при отказе.

        Raises:
            CorporateNetworkError: если DLL вернула ошибку и выдала текст ошибки.
        """
        result = self.dll.network_add_device(
            self.handle,
            domain_id.encode('utf-8'),
            device.handle,
            user.encode('utf-8')
        )

        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False

        return True

    def get_last_error(self) -> str:
        """Возвращает последнюю ошибку, полученную из DLL (в виде строки)."""
        error = self.dll.get_last_error()
        return decode_error_string(error) if error else ""

    def clear_error(self):
        """Очищает состояние последней ошибки в DLL."""
        self.dll.clear_last_error()

    def remove_device_from_network(self, device_id: str, user: str) -> bool:
        """Удаляет сущность из сети по идентификатору.

        Args:
            device_id: Идентификатор сущности для удаления.
            user: Идентификатор пользователя, выполняющего операцию.

        Returns:
            True при успехе, False при отказе.

        Notes:
            После успешного удаления Python-список `self.devices` пересобирается,
            чтобы удалить удаленную сущность (на основе `get_info()`).

        Raises:
            CorporateNetworkError: если DLL вернула ошибку и выдала текст ошибки.
        """
        result = self.dll.network_remove_device(
            self.handle,
            device_id.encode('utf-8'),
            user.encode('utf-8')
        )

        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False

        devices_to_keep = []
        for d in self.devices:
            try:
                info = d.get_info()
                if info and info.get('id') != device_id:
                    devices_to_keep.append(d)
            except:
                pass
        self.devices = devices_to_keep
        return True
