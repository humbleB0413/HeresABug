import QtQuick
import HeresABug

// The page-independent MetaButton regions of the "example_from_scratch" project, stacked as Main.qml
// places them on the right of the window (the live event-log panel lives in Main.qml):
//   - orange box: reserved for this project's device / hardware buttons — none defined yet.
//                 (Those would be ui_id != "RES" and log ordinary events, not set the result.)
//   - blue box:   the bug-report Result submitters (PASS, FAIL) — ui_id "RES". Only a RES press
//                 flips record_controller.has_result true and enables Main.qml's Export button.
// See qml/example/MetaUi.qml for the fully-populated version.
Column {
    id: meta_ui

    property int item_height: 40
    property int box_height: 120

    spacing: 12

    // Orange box — device / hardware buttons. Empty here (this project has none yet); the framed
    // region is kept so both app dirs lay out the same way.
    Rectangle {
        width: parent.width
        height: meta_ui.box_height
        color: "transparent"
    }

    // Blue box — bug-report result.
    Rectangle {
        width: parent.width
        height: meta_ui.box_height
        color: "transparent"

        Row {
            anchors.centerIn: parent
            spacing: 16

            MetaButton {
                text: "PASS"
                width: 90
                height: meta_ui.item_height
                ui_id: "RES"
                ui_num: "01"
                ui_action: "01"
                value: "Result PASS"
            }

            MetaButton {
                text: "FAIL"
                width: 90
                height: meta_ui.item_height
                ui_id: "RES"
                ui_num: "01"
                ui_action: "02"
                value: "Result FAIL"
            }
        }
    }
}
