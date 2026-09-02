import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

// Opens an existing bug-report .xlsx file so its records can be appended to.
// excel_io_controller.load_file rejects (returns false) any file whose A1 cell isn't
// DEFAULT_COUNT_LABEL ("Total Counts"), so a wrong-format file is cancelled here rather than
// partially loaded.
Button {
    id: root
    text: "Load File"

    FileDialog {
        id: file_dialog
        fileMode: FileDialog.OpenFile
        nameFilters: ["Excel Files (*.xlsx)"]
        onAccepted: {
            if (!excel_io_controller.load_file(selectedFile)) {
                console.warn("Load File cancelled: unrecognized file format.")
            }
        }
    }

    onClicked: file_dialog.open()
}
