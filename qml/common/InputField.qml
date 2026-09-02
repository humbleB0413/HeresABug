import QtQuick
import QtQuick.Controls

// Page-scoped keyboard text input. Named InputField (not Text/TextInput/TextField) to avoid
// shadowing QtQuick's own Text/TextInput and QtQuick.Controls' TextField within this module.
// Submits its current text as an event via record_controller.record_log_from_page_ui when
// editing finishes (Enter pressed, or focus lost with an accepted value) — the input-field
// equivalent of Button.qml's onClicked.
TextField {
    id: root

    property string page_id: ""
    property string ui_id: ""
    property string ui_num: ""
    property string ui_action: ""
    // Default label; the logged value is `value` + " " + the entered text (the field's contents
    // are its state), e.g. "{003:Sample Text hello world}".
    property string value: ""

    onEditingFinished: {
        var logged = text ? (value ? value + " " + text : text) : value
        record_controller.record_log_from_page_ui(page_id, ui_id, ui_num, ui_action, logged)
    }
}
