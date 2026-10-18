import QtQuick

// Navigation button. It *is* a Button — same physical qml/common/Button.qml base, reached by the
// implicit same-directory import (NOT QtQuick.Controls' Button) — so it carries the identity
// properties page_id / ui_id / ui_num / ui_action / value and inherits Button.qml's own
// onClicked, which logs a page-scoped event via record_controller.record_log_from_page_ui.
// This file adds a second onClicked that navigates. QML signal handlers are additive across the
// inheritance chain and run base-first, so a click logs the event *then* switches page — the order
// record_controller's Delete-One bookkeeping (mark_last_event_navigated) depends on.
// A page therefore only sets properties on a NavButton; it never needs an onClicked of its own.
Button {
    id: root

    property string target_page_id: ""

    onClicked: navigation_controller.go_to_page(target_page_id)
}
