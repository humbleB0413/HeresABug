import QtQuick
import HeresABug

// The two page-independent MetaButton regions of the "example" project, stacked as Main.qml places
// them on the right of the window (the third region — the live event-log panel — lives in Main.qml,
// since its content is the same for every app dir):
//   - orange box: the device / hardware buttons (ON/OFF, STOP) — ui_id "PWR". These log ordinary
//                 page-independent events (gated by is_recording); they do NOT set the result.
//   - blue box:   the bug-report Result submitters (PASS, FAIL) — ui_id "RES". Only these call
//                 through to record::push_result, so only a PASS/FAIL press flips
//                 record_controller.has_result true and enables Main.qml's Export button. The last
//                 RES press wins. (record_log_from_meta_ui branches on ui_id — see record_controller.)
Column {
    id: meta_ui

    property int item_height: 40
    property int box_height: 120

    spacing: 12

    // Orange box — device / hardware buttons, specific to this project's hardware.
    Rectangle {
        width: parent.width
        height: meta_ui.box_height
        color: "transparent"

        Row {
            anchors.centerIn: parent
            spacing: 16

            MetaButton {
                text: "ON/OFF"
                width: 90
                height: meta_ui.item_height
                ui_id: "PWR"
                ui_num: "01"
                ui_action: "01"
                value: "Power ON/OFF"
            }

            MetaButton {
                text: "STOP"
                width: 90
                height: meta_ui.item_height
                ui_id: "PWR"
                ui_num: "01"
                ui_action: "02"
                value: "Power STOP"
            }
        }
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
