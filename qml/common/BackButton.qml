import QtQuick

// Logged back navigation — a device's own "back" key. It *is* a Button (qml/common/Button.qml, the
// same-directory import), so it carries page_id / ui_id / ui_num / ui_action / value and inherits
// Button.qml's onClicked, which logs a page-scoped event. This file adds a second onClicked
// (handlers run base-first, so the event is logged before the page changes) that pops the
// back-history like PageLoader's Back button, then tags the event just logged with the navigation
// state from before the move. Deleting that event with Delete One restores that state, undoing the Back.
// A page only sets properties on a BackButton; it never needs an onClicked of its own.
Button {
    id: root

    onClicked: {
        var previous_state = navigation_controller.navigation_state()
        if (navigation_controller.go_back())
            record_controller.mark_last_event_navigated(previous_state)
    }
}
