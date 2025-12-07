import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QTreeWidget, QTreeWidgetItem, QPushButton,
                             QLabel, QLineEdit, QTextEdit, QGroupBox, QFormLayout,
                             QTabWidget, QMessageBox, QInputDialog, QComboBox, QDialog)
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QFont
import corporate_network as cn
from datetime import datetime


class DeviceTreeWidget(QTreeWidget):
    device_selected = pyqtSignal(dict)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setHeaderLabels(['ID', 'Тип', 'MAC', 'Информация'])
        self.itemClicked.connect(self.on_item_clicked)
        self.network = None

    def set_network(self, network):
        self.network = network
        self.clear()

        if not network:
            return

        root = QTreeWidgetItem(self, ['Корневой домен', 'Domain', '', ''])

        if hasattr(network, 'devices') and network.devices:
            for device in network.devices:
                try:
                    info = device.get_info()
                    if info and info.get('id'):
                        device_id = info.get('id', 'Unknown')
                        device_type = info.get('type', cn.DeviceType.UNKNOWN)
                        mac = info.get('mac', '')
                        
                        type_names = {
                            cn.DeviceType.DATA_STORAGE: 'DataStorage',
                            cn.DeviceType.WORKSTATION: 'Workstation',
                            cn.DeviceType.PRINTER: 'Printer',
                            cn.DeviceType.DOMAIN: 'Domain',
                            cn.DeviceType.UNKNOWN: 'Unknown'
                        }
                        type_str = type_names.get(device_type, 'Unknown')
                        
                        info_str = ''
                        if device_type == cn.DeviceType.DATA_STORAGE:
                            info_str = 'Хранилище'
                        elif device_type == cn.DeviceType.WORKSTATION:
                            info_str = 'Рабочая станция'
                        elif device_type == cn.DeviceType.PRINTER:
                            info_str = 'Принтер'
                        elif device_type == cn.DeviceType.DOMAIN:
                            info_str = 'Домен'
                        
                        device_item = QTreeWidgetItem(root, [device_id, type_str, mac, info_str])
                except Exception:
                    pass

        self.expandAll()

    def on_item_clicked(self, item, column):
        device_info = {
            'id': item.text(0),
            'type': item.text(1),
            'mac': item.text(2),
            'info': item.text(3)
        }
        self.device_selected.emit(device_info)


class AddDeviceDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Добавить устройство")
        self.setModal(True)
        self.setup_ui()

    def setup_ui(self):
        layout = QVBoxLayout()

        self.type_combo = QComboBox()
        self.type_combo.addItems(["Хранилище данных", "Рабочая станция", "Принтер", "Домен"])
        self.type_combo.currentIndexChanged.connect(self.on_type_changed)

        form = QFormLayout()
        self.id_edit = QLineEdit()
        self.mac_edit = QLineEdit()

        form.addRow("ID:", self.id_edit)
        form.addRow("MAC адрес:", self.mac_edit)

        self.specific_widget = QWidget()
        self.specific_layout = QFormLayout()
        self.specific_widget.setLayout(self.specific_layout)

        button_layout = QHBoxLayout()
        self.ok_button = QPushButton("Создать")
        self.cancel_button = QPushButton("Отмена")

        self.ok_button.clicked.connect(self.accept)
        self.cancel_button.clicked.connect(self.reject)

        button_layout.addWidget(self.ok_button)
        button_layout.addWidget(self.cancel_button)

        layout.addWidget(QLabel("Тип устройства:"))
        layout.addWidget(self.type_combo)
        layout.addLayout(form)
        layout.addWidget(self.specific_widget)
        layout.addLayout(button_layout)

        self.setLayout(layout)
        self.on_type_changed(0)

    def on_type_changed(self, index):
        for i in reversed(range(self.specific_layout.count())):
            widget = self.specific_layout.itemAt(i).widget()
            if widget:
                widget.deleteLater()

        if index == 0:
            self.total_size_edit = QLineEdit()
            self.specific_layout.addRow("Общий размер (MB):", self.total_size_edit)
        elif index == 1:
            self.user_edit = QLineEdit()
            self.specific_layout.addRow("Пользователь:", self.user_edit)
        elif index == 3:
            self.admin_edit = QLineEdit()
            self.specific_layout.addRow("Администратор:", self.admin_edit)

    def get_device_data(self):
        data = {
            'id': self.id_edit.text(),
            'mac': self.mac_edit.text(),
            'type': self.type_combo.currentIndex()
        }

        if data['type'] == 0:
            data['total_size'] = float(self.total_size_edit.text())
        elif data['type'] == 1:
            data['user'] = self.user_edit.text()
        elif data['type'] == 3:
            data['admin'] = self.admin_edit.text()

        return data


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.network = None
        self._root_admin_id = None
        self._selected_device = None
        self._selected_device_info = None
        self.init_ui()

    def init_ui(self):
        self.setWindowTitle("Корпоративная сеть - Управление")
        self.setGeometry(100, 100, 1200, 800)

        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QHBoxLayout()
        central_widget.setLayout(main_layout)

        left_panel = QVBoxLayout()

        tree_group = QGroupBox("Структура сети")
        tree_layout = QVBoxLayout()
        self.tree_widget = DeviceTreeWidget()
        self.tree_widget.device_selected.connect(self.on_device_selected)
        tree_layout.addWidget(self.tree_widget)
        tree_group.setLayout(tree_layout)

        left_panel.addWidget(tree_group)

        button_layout = QHBoxLayout()
        self.create_network_btn = QPushButton("Создать сеть")
        self.add_device_btn = QPushButton("Добавить устройство")
        self.remove_device_btn = QPushButton("Удалить устройство")

        self.create_network_btn.clicked.connect(self.create_network)
        self.add_device_btn.clicked.connect(self.add_device)
        self.remove_device_btn.clicked.connect(self.remove_device)

        button_layout.addWidget(self.create_network_btn)
        button_layout.addWidget(self.add_device_btn)
        button_layout.addWidget(self.remove_device_btn)

        left_panel.addLayout(button_layout)

        right_panel = QVBoxLayout()

        self.tabs = QTabWidget()

        self.info_tab = QWidget()
        self.setup_info_tab()
        self.tabs.addTab(self.info_tab, "Информация")

        self.operations_tab = QWidget()
        self.setup_operations_tab()
        self.tabs.addTab(self.operations_tab, "Операции")

        self.log_tab = QWidget()
        self.setup_log_tab()
        self.tabs.addTab(self.log_tab, "Журнал")

        right_panel.addWidget(self.tabs)

        main_layout.addLayout(left_panel, 1)
        main_layout.addLayout(right_panel, 2)

        self.statusBar().showMessage("Готово")

    def setup_info_tab(self):
        layout = QFormLayout()

        self.device_id_label = QLabel("")
        self.device_type_label = QLabel("")
        self.device_mac_label = QLabel("")
        self.device_info_label = QLabel("")

        layout.addRow("ID:", self.device_id_label)
        layout.addRow("Тип:", self.device_type_label)
        layout.addRow("MAC:", self.device_mac_label)
        layout.addRow("Информация:", self.device_info_label)

        self.specific_info = QTextEdit()
        self.specific_info.setReadOnly(True)
        layout.addRow(self.specific_info)

        self.info_tab.setLayout(layout)

    def setup_operations_tab(self):
        layout = QVBoxLayout()

        self.storage_group = QGroupBox("Операции с хранилищем")
        storage_layout = QFormLayout()

        self.add_data_btn = QPushButton("Добавить данные")
        self.free_data_btn = QPushButton("Освободить данные")
        self.add_user_btn = QPushButton("Добавить пользователя")

        self.add_data_btn.clicked.connect(self.on_add_data)
        self.free_data_btn.clicked.connect(self.on_free_data)
        self.add_user_btn.clicked.connect(self.on_add_user)

        storage_layout.addRow(self.add_data_btn)
        storage_layout.addRow(self.free_data_btn)
        storage_layout.addRow(self.add_user_btn)

        self.storage_group.setLayout(storage_layout)
        self.storage_group.setVisible(False)

        layout.addWidget(self.storage_group)
        layout.addStretch()

        self.operations_tab.setLayout(layout)

    def setup_log_tab(self):
        layout = QVBoxLayout()

        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)

        layout.addWidget(self.log_text)

        self.log_tab.setLayout(layout)

    def create_network(self):
        admin_id, ok = QInputDialog.getText(self, "Создание сети",
                                            "Введите ID администратора корневого домена:")
        if ok and admin_id:
            try:
                self.network = cn.CorporateNetwork(admin_id)
                self._root_admin_id = admin_id
                self.tree_widget.set_network(self.network)
                self.log_message(f"Создана новая сеть с администратором: {admin_id}")
                self.statusBar().showMessage(f"Сеть создана. Администратор: {admin_id}")
            except (cn.CorporateNetworkError, FileNotFoundError, OSError) as e:
                error_msg = f"Не удалось создать сеть: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")
            except Exception as e:
                error_msg = f"Неожиданная ошибка: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")

    def add_device(self):
        if not self.network:
            QMessageBox.warning(self, "Внимание", "Сначала создайте сеть")
            return

        domain_id = ""
        if self._selected_device_info and self._selected_device_info.get('type') == 'Domain':
            selected_id = self._selected_device_info['id']
            if selected_id == 'Корневой домен' or selected_id == 'root_domain':
                domain_id = ""
            elif self._selected_device:
                try:
                    info = self._selected_device.get_info()
                    if info and info.get('id'):
                        domain_id = info.get('id')
                    else:
                        domain_id = selected_id
                except:
                    domain_id = selected_id
            else:
                domain_id = selected_id

        dialog = AddDeviceDialog(self)
        if dialog.exec_():
            data = dialog.get_device_data()

            try:
                if data['type'] == 0:
                    device = self.network.create_storage(
                        data['id'],
                        data['mac'],
                        data['total_size']
                    )
                elif data['type'] == 1:
                    device = self.network.create_workstation(
                        data['id'],
                        data['mac'],
                        data['user'],
                        int(datetime.now().timestamp())
                    )
                elif data['type'] == 2:
                    device = self.network.create_printer(data['id'], data['mac'])
                elif data['type'] == 3:
                    device = self.network.create_domain(data['id'], data['admin'])

                user = self._root_admin_id if self._root_admin_id else "admin"

                if domain_id:
                    self.log_message(f"Добавление устройства в домен: '{domain_id}'")
                else:
                    self.log_message("Добавление устройства в корневой домен")

                device_added = False
                try:
                    if self.network.add_device_to_domain(domain_id, device, user):
                        device_added = True
                        type_names = {0: 'Хранилище', 1: 'Рабочая станция', 2: 'Принтер', 3: 'Домен'}
                        type_name = type_names.get(data['type'], 'Unknown')
                        self.log_message(f"Добавлено устройство: {data['id']} (тип: {type_name})")
                        self.tree_widget.set_network(self.network)
                    else:
                        self.log_message(f"ОШИБКА: Не удалось добавить устройство {data['id']} в домен")
                except cn.CorporateNetworkError as e:
                    raise
                finally:
                    if not device_added and device in self.network.devices:
                        self.network.devices.remove(device)

            except cn.CorporateNetworkError as e:
                error_msg = f"Не удалось добавить устройство: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")
            except Exception as e:
                error_msg = f"Неожиданная ошибка при добавлении устройства: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")

    def remove_device(self):
        if not self.network:
            QMessageBox.warning(self, "Внимание", "Сначала создайте сеть")
            return

        if not self._selected_device_info:
            QMessageBox.warning(self, "Внимание", "Выберите устройство для удаления")
            return

        device_id = self._selected_device_info['id']
        
        if device_id == 'Корневой домен' or device_id == 'root_domain':
            QMessageBox.warning(self, "Внимание", "Нельзя удалить корневой домен")
            return
        
        reply = QMessageBox.question(
            self,
            "Подтверждение удаления",
            f"Вы уверены, что хотите удалить устройство '{device_id}'?",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.No
        )

        if reply == QMessageBox.Yes:
            try:
                user = self._root_admin_id if self._root_admin_id else "admin"
                if self.network.remove_device_from_network(device_id, user):
                    self.log_message(f"Удалено устройство: {device_id}")
                    self._selected_device = None
                    self._selected_device_info = None
                    self.tree_widget.set_network(self.network)
                    self.device_id_label.setText("")
                    self.device_type_label.setText("")
                    self.device_mac_label.setText("")
                    self.device_info_label.setText("")
                    self.specific_info.clear()
                    self.storage_group.setVisible(False)
                    self.statusBar().showMessage(f"Устройство {device_id} удалено")
                else:
                    error_msg = "Не удалось удалить устройство"
                    QMessageBox.critical(self, "Ошибка", error_msg)
                    self.log_message(f"ОШИБКА: {error_msg}")
            except cn.CorporateNetworkError as e:
                error_msg = f"Не удалось удалить устройство: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")
            except Exception as e:
                error_msg = f"Неожиданная ошибка при удалении устройства: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")

    def on_device_selected(self, device_info):
        self._selected_device_info = device_info
        self.device_id_label.setText(device_info['id'])
        self.device_type_label.setText(device_info['type'])
        self.device_mac_label.setText(device_info['mac'])
        self.device_info_label.setText(device_info['info'])

        self._selected_device = None
        if self.network and hasattr(self.network, 'devices'):
            for device in self.network.devices:
                try:
                    info = device.get_info()
                    if info.get('id') == device_info['id']:
                        self._selected_device = device
                        break
                except:
                    continue

        if device_info['type'] == 'DataStorage':
            self.storage_group.setVisible(True)
        else:
            self.storage_group.setVisible(False)

    def on_add_data(self):
        if not self._selected_device:
            QMessageBox.warning(self, "Внимание", "Выберите хранилище для добавления данных")
            return

        if self._selected_device_info['type'] != 'DataStorage':
            QMessageBox.warning(self, "Внимание", "Выбранное устройство не является хранилищем")
            return

        size, ok = QInputDialog.getDouble(
            self,
            "Добавить данные",
            "Введите размер данных (MB):",
            min=0.1,
            decimals=2
        )

        if ok:
            try:
                if self._selected_device.storage_add_data(size):
                    self.log_message(f"Добавлено {size} MB данных в хранилище {self._selected_device_info['id']}")
                    self.statusBar().showMessage(f"Добавлено {size} MB данных")
                else:
                    error_msg = "Не удалось добавить данные"
                    QMessageBox.critical(self, "Ошибка", error_msg)
                    self.log_message(f"ОШИБКА: {error_msg}")
            except cn.CorporateNetworkError as e:
                error_msg = f"Ошибка при добавлении данных: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")
            except Exception as e:
                error_msg = f"Неожиданная ошибка: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")

    def on_free_data(self):
        if not self._selected_device:
            QMessageBox.warning(self, "Внимание", "Выберите хранилище для освобождения данных")
            return

        if self._selected_device_info['type'] != 'DataStorage':
            QMessageBox.warning(self, "Внимание", "Выбранное устройство не является хранилищем")
            return

        size, ok = QInputDialog.getDouble(
            self,
            "Освободить данные",
            "Введите размер данных для освобождения (MB):",
            min=0.1,
            decimals=2
        )

        if ok:
            try:
                if self._selected_device.storage_free_data(size):
                    self.log_message(f"Освобождено {size} MB данных в хранилище {self._selected_device_info['id']}")
                    self.statusBar().showMessage(f"Освобождено {size} MB данных")
                else:
                    error_msg = "Не удалось освободить данные"
                    QMessageBox.critical(self, "Ошибка", error_msg)
                    self.log_message(f"ОШИБКА: {error_msg}")
            except cn.CorporateNetworkError as e:
                error_msg = f"Ошибка при освобождении данных: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")
            except Exception as e:
                error_msg = f"Неожиданная ошибка: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")

    def on_add_user(self):
        if not self._selected_device:
            QMessageBox.warning(self, "Внимание", "Выберите хранилище для добавления пользователя")
            return

        if self._selected_device_info['type'] != 'DataStorage':
            QMessageBox.warning(self, "Внимание", "Выбранное устройство не является хранилищем")
            return

        user_id, ok = QInputDialog.getText(
            self,
            "Добавить пользователя",
            "Введите ID пользователя:"
        )

        if ok and user_id:
            try:
                if self._selected_device.storage_add_trusted_user(user_id):
                    self.log_message(f"Добавлен доверенный пользователь {user_id} в хранилище {self._selected_device_info['id']}")
                    self.statusBar().showMessage(f"Добавлен пользователь {user_id}")
                else:
                    error_msg = "Не удалось добавить пользователя"
                    QMessageBox.critical(self, "Ошибка", error_msg)
                    self.log_message(f"ОШИБКА: {error_msg}")
            except cn.CorporateNetworkError as e:
                error_msg = f"Ошибка при добавлении пользователя: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")
            except Exception as e:
                error_msg = f"Неожиданная ошибка: {str(e)}"
                QMessageBox.critical(self, "Ошибка", error_msg)
                self.log_message(f"ОШИБКА: {error_msg}")

    def log_message(self, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.log_text.append(f"[{timestamp}] {message}")


def main():
    app = QApplication(sys.argv)
    font = QFont()
    font.setPointSize(9)
    app.setFont(font)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()