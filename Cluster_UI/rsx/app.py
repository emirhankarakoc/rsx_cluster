import sys
import time
import math
from pathlib import Path
from PySide6.QtCore import QObject, Property, Signal, Slot, QTimer, QUrl
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
from .model import Telemetry
from .demo import Demo
from .serial_source import SerialSource

class Controller(QObject):
    changed = Signal()
    portsChanged = Signal()
    def __init__(self):
        super().__init__()
        self.model = Telemetry()
        self.demo = Demo()
        self.serial = SerialSource()
        self.is_demo = True
        self.message = 'Donanim gerektirmeyen örnek senaryo'
        self.port_names = self.serial.ports()
        self.data = {}
        self.display_rpm = 0.0
        self.display_speed = None
        self.rpm_max = 9000
        self.redline = 7000
        self.unit_kmh = False
        self.smoothing_ms = 85.0
        self.previous = time.monotonic()
        self.serial.received.connect(lambda value:self.model.accept(value,time.monotonic()))
        self.serial.ended.connect(self.clear)
        self.serial.status.connect(self.set_status)
        self.timer = QTimer(self)
        self.timer.setTimerType(__import__('PySide6.QtCore',fromlist=['Qt']).Qt.TimerType.PreciseTimer)
        self.timer.timeout.connect(self.tick)
        self.timer.start(16)
        self.tick()
    def clear(self):
        self.model.reset()
        self.display_rpm = 0
        self.display_speed = None
        if not self.is_demo:
            self.data = self.model.snapshot(time.monotonic())
            self.changed.emit()
    def set_status(self,text):
        self.message = text
        self.changed.emit()
    def tick(self):
        now = time.monotonic()
        dt = min(now-self.previous,0.1)
        self.previous = now
        self.data = self.demo.step(dt) if self.is_demo else self.model.snapshot(now)
        target = self.data.get('rpm')
        if target is None:
            self.display_rpm = 0
        else:
            alpha = 1 if self.smoothing_ms <= 0 else 1-math.exp(-dt/(self.smoothing_ms/1000))
            self.display_rpm += (target-self.display_rpm)*alpha
        speed = self.data.get('speed_mph')
        speed = speed*(1.609344 if self.unit_kmh else 1) if speed is not None else None
        if speed is None:
            self.display_speed = None
        elif self.display_speed is None or abs(speed-self.display_speed) >= 0.65:
            self.display_speed = int(math.floor(speed+0.5))
        self.changed.emit()
    @Property('QVariantMap',notify=changed)
    def state(self):
        values = dict(self.data)
        values.update(rpmShown=self.display_rpm,speedText='—' if self.display_speed is None else str(self.display_speed),demo=self.is_demo,status=self.message,rpmMax=self.rpm_max,redline=self.redline,unit='km/h' if self.unit_kmh else 'MPH',paused=self.demo.paused,manual=self.demo.manual)
        return values
    @Property('QStringList',notify=portsChanged)
    def ports(self):
        return self.port_names
    @Slot()
    def refresh(self):
        self.port_names = self.serial.ports()
        self.portsChanged.emit()
    @Slot(str,int,str)
    def connectPort(self,name,baud,mode):
        self.is_demo = False
        self.serial.open(name,baud,mode)
        self.changed.emit()
    @Slot()
    def disconnectPort(self):
        self.serial.close()
        self.set_status('Baglanti kesildi')
    @Slot()
    def useDemo(self):
        self.serial.close()
        self.is_demo = True
        self.set_status('Örnek senaryo')
    @Slot(bool)
    def pause(self,value): self.demo.paused = value
    @Slot(bool)
    def manual(self,value): self.demo.manual = value
    @Slot(float,float)
    def manualValues(self,rpm,speed):
        self.demo.rpm,self.demo.speed = rpm,speed
    @Slot(str,bool)
    def lamp(self,key,value):
        if key in self.demo.lamps: self.demo.lamps[key] = value
    @Slot(float,float,bool,float)
    def configure(self,maximum,redline,kmh,smoothing):
        self.rpm_max = max(1000,min(30000,maximum))
        self.redline = max(0,min(self.rpm_max,redline))
        self.unit_kmh = kmh
        self.smoothing_ms = max(0,min(500,smoothing))
        self.display_speed = None

def main():
    app = QGuiApplication(sys.argv)
    app.setApplicationName('RSX Digital Cluster')
    engine = QQmlApplicationEngine()
    controller = Controller()
    engine.rootContext().setContextProperty('cluster',controller)
    engine.load(QUrl.fromLocalFile(str(Path(__file__).with_name('Main.qml'))))
    if not engine.rootObjects():
        return 1
    return app.exec()

if __name__ == '__main__':
    sys.exit(main())
