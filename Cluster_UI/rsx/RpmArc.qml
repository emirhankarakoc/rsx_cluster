import QtQuick
import QtQuick.Shapes
import "RpmGeometry.js" as Geometry

Item {
    id: arc
    property real maximum: 9000
    property real redline: 7000
    property real rpm: 0
    property bool valid: true
    // These bindings depend only on settings; changing rpm cannot rebuild paths.
    readonly property var layout: Geometry.geometry(maximum,redline)
    readonly property real fraction: Math.max(0,Math.min(1,rpm/maximum))
    implicitWidth: 1200
    implicitHeight: 235
    component Band: Shape {
        id: band
        required property string outline
        property color ink: "#290b0e"
        width: 1200; height: 235
        ShapePath { strokeWidth: -1; fillColor: band.ink
            PathSvg { path: band.outline }
        }
    }
    Repeater {
        model: arc.layout.segments
        Band { required property var modelData; outline: modelData.background }
    }
    Item {
        width: arc.valid && arc.rpm > 0 ? Geometry.edge(arc.fraction) : 0
        height: parent.height
        clip: true
        Repeater {
            model: arc.layout.segments
            Item {
                required property var modelData
                Band { outline: modelData.amber; ink: "#e52b36" }
                Band { outline: modelData.red; ink: "#ff7469" }
            }
        }
    }
    Repeater {
        model: arc.layout.ticks
        Item {
            required property var modelData
            Shape {
                width: 1200; height: 235
                ShapePath {
                    strokeColor: "#ef3440"; strokeWidth: modelData.major ? 2 : 1
                    startX: modelData.x; startY: modelData.y
                    PathLine { x: modelData.x2; y: modelData.y2 }
                }
            }
            Text {
                x: modelData.labelX-width/2; y: modelData.labelY-height/2
                text: modelData.label; color: "#ff3945"
                font.pixelSize: modelData.major ? 18 : 15
                font.bold: modelData.major
            }
        }
    }
}
