import QtQuick

// Page-scoped rounded slider/switch — a custom look-alike of the CSS
// "<label class='switch'><input type='checkbox'><span class='slider round'></span></label>"
// pattern, not QtQuick.Controls.Switch. ui_action isn't a settable property here (unlike
// Button.qml): the whole point of a toggle is that its state *is* the action, so it's derived
// from `checked` — "00" off / "01" on — and logged via record_controller.record_log_from_page_ui
// on every toggle.
Item {
    id: root

    property string page_id: ""
    property string ui_id: ""
    property string ui_num: ""
    // Default label; the logged value is `value` + " On"/" Off" (the toggle's state is its action).
    property string value: ""

    property bool checked: false

    readonly property string ui_action: checked ? "01" : "00"

    width: 50
    height: 26

    Rectangle {
        id: track
        anchors.fill: parent
        radius: height / 2
        color: root.checked ? "#2196f3" : "#cccccc"

        Behavior on color { ColorAnimation { duration: 150 } }
    }

    Rectangle {
        id: thumb
        width: track.height - 4
        height: track.height - 4
        radius: height / 2
        color: "white"
        y: 2
        x: root.checked ? track.width - width - 2 : 2

        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.checked = !root.checked
            var state = root.checked ? "On" : "Off"
            var logged = root.value ? root.value + " " + state : state
            record_controller.record_log_from_page_ui(root.page_id, root.ui_id, root.ui_num, root.ui_action, logged)
        }
    }
}
