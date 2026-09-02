import QtQuick
import HeresABug

// Reference composition for page "001" (registered in pages.json). Shows how a concrete page pulls
// in qml/common elements and tags its own controls with an event id.
//
// Event-id scheme (see common.hpp, EVENT_ID_LENGTH = 10):
//   page_id(3)   - this page's id, "001"
//   ui_id(3)     - control-type mnemonic: BTN button/navigation, TGL toggle, TXT text field
//   ui_num(2)    - per-(page, ui_id) instance index, "00"-based, in document order
//   ui_action(2) - "00" unless the control derives its own (ToggleButton does: "01" on / "00" off)
//   value        - a default label; record_controller logs it as "{page_id:value}", and
//                  state-varying controls append a suffix ("{003:Sample Toggle On}")
//
// A NavButton needs no onClicked here: it is a Button (see NavButton.qml), so Button.qml's own
// onClicked logs the event and NavButton.qml's onClicked navigates — both fire, log first.
Rectangle {
    anchors.fill: parent
    color: "#eef6ff"

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "Example Page One"
        }

        Button {
            text: "Log sample event"
            page_id: "001"
            ui_id: "BTN"
            ui_num: "00"
            ui_action: "00"
            value: text
        }

        NavButton {
            text: "Go to Page Two"
            page_id: "001"
            ui_id: "BTN"
            ui_num: "01"
            ui_action: "00"
            value: text
            target_page_id: "002"
        }
    }
}
