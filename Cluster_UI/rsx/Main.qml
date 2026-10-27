import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: win
    visible: true
    width: 1280; height: 720
    minimumWidth: 800; minimumHeight: 480
    color: "#000000"
    palette.window: "#000000"
    palette.base: "#100305"
    palette.alternateBase: "#1c080b"
    palette.button: "#23080c"
    palette.text: "#ff3945"
    palette.windowText: "#ff3945"
    palette.buttonText: "#ff3945"
    palette.highlight: "#ce2430"
    palette.highlightedText: "#ffffff"
    title: "RSX Digital Cluster"
    property var s: cluster.state
    function toggleFull() { visibility = visibility === Window.FullScreen ? Window.Windowed : Window.FullScreen }
    Shortcut { sequence: "F11"; onActivated: win.toggleFull() }
    Shortcut { sequence: "Escape"; onActivated: { if (win.visibility === Window.FullScreen) win.visibility = Window.Windowed; settings.close() } }
    component Lamp: Item {
        property string field
        property string label
        property color activeColor: "#ffb52e"
        width: 66; height: 64
        property var value: win.s[field]
        Image { anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter; width: 35; height: 30; source: "../assets/"+parent.field+".svg"; opacity: parent.value === true ? 1 : parent.value === false ? 0.17 : 0.42 }
        Rectangle { anchors.top: parent.top; anchors.right: parent.right; width: 7; height: 7; radius: 3.5; color: parent.value === true ? parent.activeColor : "transparent" }
        Text { anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter; text: parent.label + (parent.value === undefined || parent.value === null ? " ?" : ""); color: parent.value === false ? "#9d2932" : "#ff3945"; font.pixelSize: 10; font.bold: true }
    }
    Item {
        id: dashboard
        width: 1200; height: 640
        anchors.centerIn: parent
        scale: Math.min(win.width/1280,win.height/720)
        Row { x: 0; y: 0; spacing: 12
            Lamp { field: "cel"; label: "CEL" }
            Lamp { field: "srs"; label: "SRS"; activeColor: "#ff5146" }
            Lamp { field: "charge"; label: "CHARGE"; activeColor: "#ff5146" }
        }
        Row { anchors.horizontalCenter: parent.horizontalCenter; y: 0; spacing: 12
            Lamp { field: "left"; label: "LEFT"; activeColor: "#49ed89" }
            Lamp { field: "low_beam"; label: "LOW"; activeColor: "#49ed89" }
            Lamp { field: "high_beam"; label: "HIGH"; activeColor: "#459cff" }
            Lamp { field: "right"; label: "RIGHT"; activeColor: "#49ed89" }
        }
        Row { anchors.right: parent.right; y: 0; spacing: 12
            Lamp { field: "low_fuel"; label: "FUEL" }
            Lamp { field: "brake"; label: "BRAKE"; activeColor: "#ff5146" }
            Lamp { field: "seatbelt_warning"; label: "BELT"; activeColor: "#ff5146" }
        }
        Text { y: 77; text: "ENGINE SPEED   /   RPM \u00d7 1000"; color: "#ff3945"; font.pixelSize: 13; font.letterSpacing: 2 }
        RpmArc {
            objectName: "rpmArc"
            x: 0; y: 94; width: 1200; height: 235
            maximum: win.s.rpmMax
            redline: win.s.redline
            rpm: win.s.rpmShown
            valid: win.s.rpm !== null && win.s.rpm !== undefined
        }
        Text { anchors.horizontalCenter: parent.horizontalCenter; y: 252; text: win.s.speedText; color: "#ff3945"; font.family: "Arial Black"; font.weight: Font.Black; font.pixelSize: 176 }
        Text { anchors.horizontalCenter: parent.horizontalCenter; y: 435; text: win.s.unit; color: "#ff3945"; font.pixelSize: 24; font.letterSpacing: 4 }
        Text { x: 0; y: 285; text: win.s.demo ? "DEMO" : "USB / LIVE"; color: "#ff3945"; font.pixelSize: 20; font.bold: true }
        Text { x: 0; y: 320; text: win.s.rpm === null || win.s.rpm === undefined || win.s.speed_mph === null || win.s.speed_mph === undefined ? "VERİ YOK" : ""; color: "#ff3945"; font.pixelSize: 19 }
        Text { anchors.right: parent.right; y: 305; text: win.s.rpm === null || win.s.rpm === undefined ? "— RPM" : Math.round(win.s.rpmShown)+" RPM"; color: "#ff3945"; font.pixelSize: 22 }
        Rectangle { y: 508; width: parent.width; height: 1; color: "#501017" }
        Row { y: 540; width: parent.width
            Repeater { model: [{name:"TOTAL",key:"odometer_mi"},{name:"TRIP A",key:"trip_a_mi"},{name:"TRIP B",key:"trip_b_mi"}]
                Column { required property var modelData; width: 400; spacing: 10
                    Text { text: modelData.name; color: "#ff3945"; font.pixelSize: 15; font.letterSpacing: 3 }
                    Text { text: win.s[modelData.key] === null || win.s[modelData.key] === undefined ? "—" : (win.s[modelData.key]*(win.s.unit === "km/h" ? 1.609344 : 1)).toFixed(1) + (win.s.unit === "km/h" ? " km" : " mi"); color: "#ff3945"; font.family: "Menlo"; font.pixelSize: 30 }
                }
            }
        }
    }
    Row { anchors.right: parent.right; anchors.bottom: parent.bottom; spacing: 8
        Button { text: "Tam ekran"; onClicked: win.toggleFull() }
        Button { text: "Ayarlar / Demo"; onClicked: settings.open() }
    }
    Drawer {
        id: settings
        background: Rectangle { color: "#000000"; border.color: "#501017" }
        edge: Qt.RightEdge
        width: Math.min(440,win.width)
        height: win.height
        ScrollView { anchors.fill: parent; clip: true
            ColumnLayout { width: settings.width-28; x: 14; spacing: 10
                Label { text: "BAĞLANTI & GÖRÜNTÜ"; font.bold: true }
                Label { text: win.s.status; wrapMode: Text.Wrap; Layout.fillWidth: true }
                ComboBox { id: port; model: cluster.ports; Layout.fillWidth: true }
                Button { text: "Portları yenile"; onClicked: cluster.refresh() }
                ComboBox { id: baud; model: ["115200","57600","38400","19200","9600"] }
                ComboBox { id: protocol; model: ["JSON","Legacy CSV"] }
                RowLayout {
                    Button { text: "Bağlan"; enabled: port.currentText.length > 0; onClicked: cluster.connectPort(port.currentText,parseInt(baud.currentText),protocol.currentText) }
                    Button { text: "Kes"; onClicked: cluster.disconnectPort() }
                    Button { text: "Demo"; onClicked: cluster.useDemo() }
                }
                Label { text: "Örnek başlangıç ayarları — araç teknik değeri değildir"; wrapMode: Text.Wrap; Layout.fillWidth: true }
                Label { text: "RPM ölçek üst sınırı" }
                SpinBox { id: maximum; from: 1000; to: 30000; value: 9000; stepSize: 500; editable: true }
                Label { text: "Kırmızı bölge başlangıcı" }
                SpinBox { id: red; from: 0; to: maximum.value; value: 7000; stepSize: 100; editable: true }
                CheckBox { id: kmh; text: "km/h (kapalı: MPH)" }
                Label { text: "RPM filtre zaman sabiti (ms, 0 = kapalı)" }
                SpinBox { id: smoothing; from: 0; to: 500; value: 85; stepSize: 5 }
                Button { text: "Görüntü ayarlarını uygula"; onClicked: cluster.configure(maximum.value,red.value,kmh.checked,smoothing.value) }
                Label { text: "DEMO KONTROLLERİ"; font.bold: true }
                CheckBox { text: "Senaryoyu duraklat"; enabled: win.s.demo; onToggled: cluster.pause(checked) }
                CheckBox { id: manual; text: "Elle RPM / hız"; enabled: win.s.demo; onToggled: cluster.manual(checked) }
                Label { text: "RPM: "+Math.round(rpmSlider.value) }
                Slider { id: rpmSlider; from: 0; to: maximum.value; value: 3200; enabled: win.s.demo && manual.checked; Layout.fillWidth: true; onMoved: cluster.manualValues(value,speedSlider.value) }
                Label { text: "Hız (MPH): "+Math.round(speedSlider.value) }
                Slider { id: speedSlider; from: 0; to: 180; value: 45; enabled: win.s.demo && manual.checked; Layout.fillWidth: true; onMoved: cluster.manualValues(rpmSlider.value,value) }
                Repeater { model: ["left","right","low_beam","high_beam","cel","srs","charge","low_fuel","brake","seatbelt_warning"]
                    CheckBox { required property string modelData; text: modelData; enabled: win.s.demo; onToggled: cluster.lamp(modelData,checked) }
                }
                Label { text: "İkonlar bu proje için çizilmiş yaklaşık sembollerdir."; wrapMode: Text.Wrap; Layout.fillWidth: true }
            }
        }
    }
}
