from PySide6.QtCore import QObject, Signal, QIODevice
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo
from .protocol import Framer, decode

class SerialSource(QObject):
    received = Signal(dict)
    status = Signal(str)
    ended = Signal()
    def __init__(self):
        super().__init__()
        self.port = QSerialPort(self)
        self.framer = Framer()
        self.mode = 'JSON'
        self.port.readyRead.connect(self.read)
        self.port.errorOccurred.connect(self.error)
    def ports(self):
        return [p.systemLocation() for p in QSerialPortInfo.availablePorts()]
    def close(self):
        self.port.close()
        self.framer.reset()
        self.ended.emit()
    def open(self, name, baud, mode):
        self.close()
        self.mode = mode
        self.port.setPortName(name)
        self.port.setBaudRate(baud)
        if self.port.open(QIODevice.OpenModeFlag.ReadOnly):
            self.status.emit('Bagli: '+name)
        else:
            self.status.emit(self.port.errorString())
    def read(self):
        for line in self.framer.feed(bytes(self.port.readAll())):
            value = decode(line,self.mode)
            if value is not None:
                self.received.emit(value)
    def error(self, error):
        if error in (QSerialPort.SerialPortError.ResourceError,QSerialPort.SerialPortError.DeviceNotFoundError):
            message = self.port.errorString()
            self.close()
            self.status.emit(message)
