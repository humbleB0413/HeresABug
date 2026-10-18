import QtQuick
import QtQuick.Controls

// Removes the most recently logged event from the current record. If that event was a NavButton
// click that moved pages, also returns to the page it left (navigation_controller.return_to_page),
// so undoing an event undoes the page move that came with it, and only that one.
//
// Presented as a backspace-key glyph (a left-pointing pentagon with an "x" inside).
Button {
    id: root

    property color icon_color: "#37474f"

    ToolTip.visible: hovered
    ToolTip.text: qsTr("Delete last event")

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
                ctx.lineWidth = Math.max(1.5, width * 0.09)
                ctx.lineJoin = "round"
                ctx.lineCap = "round"

                var w = width
                var h = height
                var cy = h * 0.5
                var hh = h * 0.22          // half-height of the key body
                var apex = w * 0.13        // left point
                var neck = w * 0.40        // where the point meets the rectangle
                var right = w * 0.87

                // left-pointing pentagon outline
                ctx.beginPath()
                ctx.moveTo(apex, cy)
                ctx.lineTo(neck, cy - hh)
                ctx.lineTo(right, cy - hh)
                ctx.lineTo(right, cy + hh)
                ctx.lineTo(neck, cy + hh)
                ctx.closePath()
                ctx.stroke()

                // "x" inside the rectangular part
                var xl = w * 0.52
                var xr = w * 0.78
                var xt = cy - h * 0.11
                var xb = cy + h * 0.11
                ctx.beginPath()
                ctx.moveTo(xl, xt)
                ctx.lineTo(xr, xb)
                ctx.moveTo(xr, xt)
                ctx.lineTo(xl, xb)
                ctx.stroke()
            }
        }
    }

    // Tags each NavButton event with the page it left, so a later delete knows where to return.
    Connections {
        target: navigation_controller
        function onNavigated_forward(from_page_id) { record_controller.mark_last_event_navigated(from_page_id) }
    }

    onClicked: navigation_controller.return_to_page(record_controller.delete_last_event())
}
