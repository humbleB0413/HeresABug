import QtQuick
import HeresABug

// Reference composition for page "002" — see ExamplePageOne.qml for the event-id scheme
// (page_id / ui_id / ui_num / ui_action / value).
Rectangle {
    anchors.fill: parent
    color: "#fff5e6"

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "Example Page Two"
        }

        Button {
            text: "Log sample event"
            page_id: "002"
            ui_id: "BTN"
            ui_num: "00"
            ui_action: "00"
            value: text
        }

        NavButton {
            text: "Go to Page One"
            page_id: "002"
            ui_id: "BTN"
            ui_num: "01"
            ui_action: "00"
            value: text
            target_page_id: "001"
        }

        NavButton {
            text: "Go to Page Three"
            page_id: "002"
            ui_id: "BTN"
            ui_num: "02"
            ui_action: "00"
            value: text
            target_page_id: "003"
        }
    }
}
