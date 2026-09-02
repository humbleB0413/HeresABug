import QtQuick

// The status panel's event-log line. Shows the logged events joined by ", ", but never more than
// the last `max_shown` of them — when there are more, a leading "..." stands in for the older ones
// (e.g. "..., {001:A}, {001:B}, {003:C}, {003:D}, {000:E}"). Falls back to a context-appropriate
// hint while nothing has been logged yet. Each app dir's Main.qml drops one of these into its
// status box (see CLAUDE.md's Main.qml notes on why the status box is duplicated per app dir).
Text {
    id: root

    // Cap on how many trailing events are rendered before the "..." prefix kicks in.
    readonly property int max_shown: 5

    wrapMode: Text.Wrap
    maximumLineCount: 3
    elide: Text.ElideRight

    text: {
        var events = record_controller.logged_events
        if (events.length > 0) {
            if (events.length > root.max_shown)
                return "..., " + events.slice(events.length - root.max_shown).join(", ")
            return events.join(", ")
        }
        if (!excel_io_controller.file_ready)
            return "Create or load a file to enable recording."
        if (record_controller.is_recording)
            return "Use the page controls to log events."
        return "Press Record Start to begin a session."
    }
}
