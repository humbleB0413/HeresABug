import QtQuick
import QtQuick.Controls

// Page-independent UI (page_id fixed to "000", per EVENT_ID_LENGTH's page_id(3) slot).
// Routes to record_controller.record_log_from_meta_ui, which branches on ui_id:
//   ui_id "RES" -> submits the record's single bug-report result (PASS/FAIL); this is the
//                  only thing that flips record_controller.has_result and enables Export.
//   any other   -> logs a page-independent event (device hard keys, etc.), gated by is_recording.
// Contrast Button.qml, which always logs a page-scoped event via record_log_from_page_ui.
Button {
    id: root

    property string ui_id: ""
    property string ui_num: ""
    property string ui_action: ""
    // Default label; record_controller wraps it as "{000:value}" into the record's result.
    property string value: ""

    readonly property string packed_id: "000" + ui_id + ui_num + ui_action

    onClicked: record_controller.record_log_from_meta_ui(packed_id, value)
}
