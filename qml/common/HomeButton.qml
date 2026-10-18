import QtQuick

// Logged back navigation to a home page — a device's "home" key, or the Exit of a page shared by
// several callers. Like BackButton it *is* a Button (qml/common/Button.qml), so Button.qml's
// onClicked logs a page-scoped event first; this file's onClicked then goes back in one step to
// whichever of target_page_ids was visited most recently, popping every back-history entry above
// it (navigation_controller.go_back_to), and tags the event just logged with the navigation state
// from before the move. Deleting that event with Delete One restores that state — the popped
// history included — undoing the Home. A single home is a one-element list: target_page_ids: ["005"].
// A page only sets properties on a HomeButton; it never needs an onClicked of its own.
Button {
    id: root

    property var target_page_ids: []

    onClicked: {
        var previous_state = navigation_controller.navigation_state()
        if (navigation_controller.go_back_to(target_page_ids))
            record_controller.mark_last_event_navigated(previous_state)
    }
}
