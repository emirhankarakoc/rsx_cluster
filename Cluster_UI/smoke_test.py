"""Optional Qt smoke test; requires installed PySide6. Run from project root."""
import os
os.environ.setdefault('QT_QPA_PLATFORM','offscreen')
import sys
from pathlib import Path
from PySide6.QtCore import QObject, QTimer, QUrl
from PySide6.QtGui import QGuiApplication, QWindow
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtQuick import QQuickWindow
from rsx.app import Controller

app = QGuiApplication(sys.argv)
engine = QQmlApplicationEngine()
controller = Controller()
engine.rootContext().setContextProperty('cluster',controller)
errors = []
engine.warnings.connect(lambda warnings: errors.extend(w.toString() for w in warnings))
engine.load(QUrl.fromLocalFile(str(Path('rsx/Main.qml').resolve())))
assert engine.rootObjects(), 'QML failed to load'
window = engine.rootObjects()[0]
initial = controller.demo.time
failure = []

def check():
    try:
        assert controller.demo.time > initial
        assert controller.state['demo']
        assert controller.state['rpmShown'] > 0
        window.resize(800,480)
        window.showFullScreen()
        assert window.visibility() == QWindow.Visibility.FullScreen
        window.showNormal()
        assert window.visibility() != QWindow.Visibility.FullScreen
        window.resize(1280,720)
        QTimer.singleShot(200, finish)
    except Exception as exc:
        failure.append(str(exc)); app.quit()

cases = [(rpm,w,h) for w,h in [(1280,720),(800,480),(1600,900)]
         for rpm in (1000,3500,6800,8000)]
arc_layout = None

def finish():
    controller.configure(8000,6800,False,0)
    controller.manual(True)
    controller.pause(True)
    capture_next()

def capture_next():
    if not cases:
        if errors:
            failure.append('\n'.join(errors))
        app.quit()
        return
    rpm,w,h = cases.pop(0)
    controller.manualValues(rpm,45)
    controller.tick()
    window.resize(w,h)
    def capture():
        try:
            arc = window.findChild(QObject,'rpmArc')
            assert arc is not None
            assert abs(arc.property('rpm')-rpm) < 0.001
            assert arc.property('maximum') == 8000
            assert arc.property('redline') == 6800
            filename = f'preview-{rpm}-{w}x{h}.png'
            assert window.grabWindow().save(filename)
        except Exception as exc:
            failure.append(str(exc))
        capture_next()
    QTimer.singleShot(150,capture)

QTimer.singleShot(1200,check)
app.exec()
assert not failure, '\n'.join(failure)
print('Qt smoke passed; 12 RPM/size captures written (offscreen, no FPS claim).')
