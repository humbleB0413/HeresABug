import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root

    padding: 8 

    implicitWidth:  Math.max(30, implicitContentWidth  + leftPadding + rightPadding)
    implicitHeight: Math.max(20, implicitContentHeight + topPadding  + bottomPadding)

    property string page_id: ""
    property string ui_id: ""
    property string ui_num: ""
    property string ui_action: ""
    // Default label for this control in the event log; record_controller wraps it as
    // "{page_id:value}". Single-action controls log it as-is; state-varying ones append a suffix.
    property string value: ""

    onClicked: record_controller.record_log_from_page_ui(page_id, ui_id, ui_num, ui_action, value)
}