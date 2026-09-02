import QtQuick
import QtQuick.Window
import HeresABug

// This example project's concrete window composition — the "Main" type CMakeLists.txt registers
// for whichever qml/<dir> HERESABUG_APP_DIR currently points at. Built from qml/common/'s reusable
// pieces; the specific arrangement (which pages exist, what the device buttons mean, how the panels
// are laid out) is this project's own design. A different project swaps in its own Main.qml.
Window {
    id: root
    width: 740
    height: 500
    visible: true
    title: qsTr("HeresABug")
    color: "white"

    // Every button composed into this window renders at this height, so the toolbars line up.
    readonly property int button_height: 40

    // Top-left: file operations.
    Row {
        id: file_bar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 16
        spacing: 8

        NewFileButton {
            width: 80
            height: button_height
        }

        LoadFileButton {
            width: 80
            height: button_height
        }
    }

    // Top-right: record operations.
    Row {
        id: record_bar
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 16
        spacing: 16

        RecordToggleButton {
            width: button_height
            height: button_height
        }

        DeleteOneButton {
            width: button_height
            height: button_height
        }

        ClearRecordButton {
            width: button_height
            height: button_height
        }

        ExportButton {
            width: 80
            height: button_height
        }
    }

    // Keeps record_controller's navigation bookkeeping in sync so Delete One can tell whether the
    // event it's undoing coincided with a page move.
    Connections {
        target: navigation_controller
        function onCurrent_page_changed() { record_controller.mark_navigation_occurred() }
    }

    // Left, full height: the page box. Only this region changes on navigation; PageLoader overlays
    // its own back button.
    Rectangle {
        id: page_box
        anchors.top: file_bar.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 16
        width: 260
        border.color: "black"
        border.width: 1
        color: "transparent"

        PageLoader {
            anchors.fill: parent
        }
    }

    // Right, black box: live view of the current in-progress record — event count, the logged
    // values in the "{page_id:value}" form they'll be written to the Excel Events column, and the
    // current Result-column value. Common to every app dir, so it lives here rather than in MetaUi.
    Rectangle {
        id: status_box
        anchors.top: record_bar.bottom
        anchors.left: page_box.right
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        height: 120
        radius: 4
        border.width: 2
        border.color: "black"
        color: "transparent"

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 4

            Text {
                font.bold: true
                text: record_controller.is_recording ? "Recording" : "Not recording"
            }

            Text {
                text: "Events logged: " + record_controller.logged_events.length
            }

            EventLogText {
                width: status_box.width - 20
            }

            Text {
                text: "Result: " + (record_controller.current_result !== "" ? record_controller.current_result : "(not set)")
            }
        }
    }

    // Right, below the status box: the MetaButton regions (orange = device buttons, blue = result).
    MetaUi {
        id: meta_ui
        item_height: root.button_height
        anchors.top: status_box.bottom
        anchors.left: page_box.right
        anchors.right: parent.right
        anchors.topMargin: 12
        anchors.leftMargin: 16
        anchors.rightMargin: 16
    }
}
