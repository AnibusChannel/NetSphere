import ctypes
import os
from ctypes import c_void_p, c_char_p, c_int, c_double, c_longlong, POINTER, Structure, byref, c_char, cast, addressof
from enum import IntEnum
from datetime import datetime
from typing import Optional, List, Dict, Any


class DeviceType(IntEnum):
    UNKNOWN = 0
    DATA_STORAGE = 1
    WORKSTATION = 2
    PRINTER = 3
    DOMAIN = 4


class DeviceInfo(Structure):
    _fields_ = [
        ("id", ctypes.c_char * 256),
        ("mac", ctypes.c_char * 256),
        ("type", c_int),
        ("data", ctypes.c_byte * 256)
    ]


def load_dll(dll_path: str = None):
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
    pass


def decode_error_string(error_ptr):
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
    def __init__(self, handle: c_void_p, dll):
        self.handle = handle
        self.dll = dll

    def __del__(self):
        if self.handle:
            self.dll.device_destroy(self.handle)

    def storage_add_data(self, size: float) -> bool:
        result = self.dll.storage_add_data(self.handle, c_double(size))
        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False
        return True

    def storage_free_data(self, size: float) -> bool:
        result = self.dll.storage_free_data(self.handle, c_double(size))
        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False
        return True

    def storage_add_trusted_user(self, user: str) -> bool:
        result = self.dll.storage_add_trusted_user(self.handle, user.encode('utf-8'))
        if not result:
            error = self.dll.get_last_error()
            if error:
                error_str = decode_error_string(error)
                raise CorporateNetworkError(error_str)
            return False
        return True

    def get_info(self) -> Dict[str, Any]:
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
    def __init__(self, root_admin_id: str, dll_path: str = None):
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
        if self.handle:
            self.dll.network_destroy(self.handle)

    def create_domain(self, domain_id: str, admin_id: str) -> NetworkEntity:
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
        error = self.dll.get_last_error()
        return decode_error_string(error) if error else ""

    def clear_error(self):
        self.dll.clear_last_error()

    def remove_device_from_network(self, device_id: str, user: str) -> bool:
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