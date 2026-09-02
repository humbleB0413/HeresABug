import QtQuick
import QtQuick.Controls

// Toggles whether page-scoped events (Button/InputField clicks) are captured into the current
// record. Gates record_controller.record_log_from_page_ui via record_controller.is_recording,
// rather than logging anything itself. Disabled until a destination .xlsx file exists
// (excel_io_controller.file_ready) — there's nothing to record into otherwise.
//
// Navigation on toggle: turning recording *off* stamps the current page as the resume anchor
// (navigation_controller.mark_recording_anchor); turning it back *on* returns there by popping the
// back-stack (navigation_controller.resume_to_recording_anchor), so wandering pages while stopped
// is unwound rather than stacked. The very first session (no anchor yet) still starts at page one.
//
// Presented as a camcorder-style record control: a solid red disc while idle, a red square
// (the universal "stop" glyph) while recording — the icon *is* the label, per CODING_GUIDELINES.md.
Button {
    id: root

    property color icon_color: "#e53935"

    enabled: excel_io_controller.file_ready

    ToolTip.visible: hovered
    ToolTip.text: record_controller.is_recording ? qsTr("Stop recording") : qsTr("Start recording")

    contentItem: Item {
        implicitWidth: 22
        implicitHeight: 22
        opacity: root.enabled ? 1.0 : 0.35

        Canvas {
            id: glyph
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: width

            readonly property bool recording: record_controller.is_recording

            onRecordingChanged: requestPaint()
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()
            Component.onCompleted: requestPaint()

            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()
                ctx.fillStyle = root.icon_color

                var s = width * 0.62
                var x = (width - s) / 2
                var y = (height - s) / 2

                if (recording) {
                    ctx.fillRect(x, y, s, s)
                } else {
                    ctx.beginPath()
                    ctx.arc(width / 2, height / 2, s / 2, 0, 2 * Math.PI)
                    ctx.fill()
                }
            }
        }
    }

    onClicked: {
        record_controller.toggle_recording()
        if (record_controller.is_recording) {
            navigation_controller.resume_to_recording_anchor()
        } else {
            navigation_controller.mark_recording_anchor()
        }
    }
}
