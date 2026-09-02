import QtQuick
import QtQuick.Controls

// Finalizes the current record and writes it into the currently selected .xlsx file. Disabled
// until a file has been created/loaded (excel_io_controller.file_ready) AND the record has a
// bug-report verdict (record_controller.has_result — set only by a ui_id "RES" MetaButton, i.e.
// PASS/FAIL; device buttons and Back navigation deliberately don't set it). Exporting with no
// destination file, or before a PASS/FAIL verdict, isn't a meaningful action.
Button {
    id: root
    text: "Export"
    enabled: excel_io_controller.file_ready && record_controller.has_result

    // Finalize + persist, then return to the app's initial state: record_submit() clears the
    // record queue and leaves recording; go_to_initial_page() resets navigation (clears history,
    // jumps to the first page). Composing several single-responsibility controller calls in one
    // handler follows PageLoader.qml's back-button precedent.
    onClicked: {
        excel_io_controller.add_record(record_controller.record_submit())
        excel_io_controller.export_records()
        navigation_controller.go_to_initial_page()
    }
}
