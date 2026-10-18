import QtQuick

// Logged back navigation — a device's own "back" key. It *is* a Button (qml/common/Button.qml, the
// same-directory import), so it carries page_id / ui_id / ui_num / ui_action / value and inherits
// Button.qml's onClicked, which logs a page-scoped event. This file adds a second onClicked
// (handlers run base-first, so the event is logged before the page changes) that pops the
// back-history like PageLoader's Back button, then tags the event just logged with the page it left.
// Deleting that event with Delete One therefore moves forward to that page again, undoing the Back.
// A page only sets properties on a BackButton; it never needs an onClicked of its own.
Button {
    id: root

    onClicked: {
        var from_page_id = navigation_controller.current_page_id
        if (navigation_controller.go_back())
            record_controller.mark_last_event_navigated(from_page_id)
    }
}
