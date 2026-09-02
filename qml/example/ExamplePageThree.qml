import QtQuick
import HeresABug

// Reference composition for page "003" (see ExamplePageOne.qml for the event-id scheme),
// demonstrating ToggleButton and InputField. Reached via Main.qml's PageLoader (which overlays a
// global back button), so navigating here and using that back button (rather than the NavButton
// below) exercises navigation_controller's history/go_back() path.
Rectangle {
    anchors.fill: parent
    color: "#eefbea"

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "Example Page Three"
        }

        ToggleButton {
            anchors.horizontalCenter: parent.horizontalCenter
            page_id: "003"
            ui_id: "TGL"
            ui_num: "00"
            // ToggleButton appends " On"/" Off" itself → logs "{003:Sample Toggle On}".
            value: "Sample Toggle"
        }

        InputField {
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: "Log sample text"
            page_id: "003"
            ui_id: "TXT"
            ui_num: "00"
            ui_action: "00"
            // InputField appends the entered text → logs "{003:Sample Text <what was typed>}".
            value: "Sample Text"
        }

        NavButton {
            text: "Go to Page One"
            page_id: "003"
            ui_id: "BTN"
            ui_num: "00"
            ui_action: "00"
            value: text
            target_page_id: "001"
        }
    }
}
