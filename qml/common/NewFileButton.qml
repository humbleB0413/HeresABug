import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

// Creates a fresh bug-report .xlsx file via a native Save dialog, then hands the chosen path to
// excel_io_controller.create_file so the file gets the DEFAULT_COUNT_LABEL/DEFAULT_COUNT_INFO
// layout excel_io::load() later validates against.
Button {
    id: root
    text: "New File"

    FileDialog {
        id: file_dialog
        fileMode: FileDialog.SaveFile
        nameFilters: ["Excel Files (*.xlsx)"]
        defaultSuffix: "xlsx"
        onAccepted: excel_io_controller.create_file(selectedFile)
    }

    onClicked: file_dialog.open()
}
