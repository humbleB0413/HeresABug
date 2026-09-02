import QtQuick
import QtQuick.Controls

// Owns "how to swap the visible page": renders whichever page navigation_controller.current_page_type
// currently names inside this Item's own bounds, leaving everything around it untouched. Place this
// where only a sub-region of the screen should change on navigation, not the whole window. Also
// overlays a back button whose sole job is navigation — it calls navigation_controller.go_back()
// and nothing else. (It used to be a MetaButton, which also submitted the record's result on every
// press; that let a record be exported without a real PASS/FAIL verdict, so back navigation no
// longer touches the record at all — only ui_id "RES" controls set the result now.)
// (Formerly a separate PopUp.qml composing this file; merged in since PopUp had come to be a strict
// superset of this file's responsibility, and no page embeds this itself — see the CMakeLists.txt/
// Main.qml notes in CLAUDE.md on why nesting this inside a page would recurse.)
//
// Visited pages are created once and kept alive (hidden, not destroyed) rather than recreated on
// every visit, so a page's own local UI state (e.g. a ToggleButton's checked) survives leaving and
// returning to it. Creation is lazy — only pages actually visited exist in page_instances, so this
// stays cheap regardless of how many page_ids pages.json defines (up to 999, per EVENT_ID_LENGTH's
// page_id(3) slot).
Item {
    id: root

    // page_id -> already-instantiated page Item; never removed, so a page's state is preserved
    // for the lifetime of the app once first visited
    property var page_instances: ({})
    property Item current_page_item: null

    Item {
        id: page_container
        anchors.fill: parent
    }

    // Navigation only — does not log an event or submit a result (see the header comment).
    Button {
        id: back_button
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 8
        text: "← Back"
        onClicked: navigation_controller.go_back()
    }

    function refresh_page() {
        var page_id = navigation_controller.current_page_id
        var type_name = navigation_controller.current_page_type

        if (root.current_page_item) {
            root.current_page_item.visible = false
        }

        if (!type_name) {
            root.current_page_item = null
            return
        }

        var existing = root.page_instances[page_id]
        if (existing) {
            existing.visible = true
            root.current_page_item = existing
            return
        }

        var component = Qt.createComponent("HeresABug", type_name)

        if (component.status === Component.Error) {
            console.error("PageLoader: failed to load page '" + type_name + "': " + component.errorString())
            root.current_page_item = null
            return
        }

        var page_item = component.createObject(page_container, { "anchors.fill": page_container })
        root.page_instances[page_id] = page_item
        root.current_page_item = page_item
    }

    Connections {
        target: navigation_controller
        function onCurrent_page_changed() { root.refresh_page() }
    }

    Component.onCompleted: refresh_page()
}
