import QtQuick
import QtQuick.Controls

// Resets the session to the app's start state without exporting: the in-progress record (all
// logged events and any result) is emptied, recording stops, and navigation returns to the first page.
//
// Presented as a camcorder-style "discard" control: a trash-can glyph.
Button {
    id: root

    property color icon_color: "#37474f"

    ToolTip.visible: hovered
    ToolTip.text: qsTr("Clear current record")

    contentItem: Item {
        implicitWidth: 22
        implicitHeight: 22

        Canvas {
            id: glyph
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: width

            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()
            Component.onCompleted: requestPaint()

            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()
                ctx.strokeStyle = root.icon_color
                ctx.fillStyle = root.icon_color
                ctx.lineWidth = Math.max(1.5, width * 0.07)
                ctx.lineCap = "round"

                var w = width
                var h = height

                // handle
                ctx.fillRect(w * 0.42, h * 0.17, w * 0.16, h * 0.07)
                // lid
                ctx.fillRect(w * 0.18, h * 0.26, w * 0.64, h * 0.08)
                // can body
                ctx.strokeRect(w * 0.27, h * 0.38, w * 0.46, h * 0.42)
                // ribs
                ctx.beginPath()
                ctx.moveTo(w * 0.40, h * 0.44)
                ctx.lineTo(w * 0.40, h * 0.74)
                ctx.moveTo(w * 0.50, h * 0.44)
                ctx.lineTo(w * 0.50, h * 0.74)
                ctx.moveTo(w * 0.60, h * 0.44)
                ctx.lineTo(w * 0.60, h * 0.74)
                ctx.stroke()
            }
        }
    }

    // Discard, then return to the app's initial state, mirroring ExportButton: clear_record() empties
    // the record and leaves recording; go_to_initial_page() clears the back-history and the resume
    // anchor, rebuilds the pages fresh, and jumps to the first page.
    onClicked: {
        record_controller.clear_record()
        navigation_controller.go_to_initial_page()
    }
}
