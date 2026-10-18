/**
 * @file record_controller.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief QObject wrapper exposing a `record` instance to QML
 * @version 0.1
 * @date 2026-08-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "common.hpp"
#include "record.hpp"

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief QML-facing adapter around one `record`: the recording on/off gate, the log entry points
 *        for page and meta UI, a live view of the in-progress record for the status panel, and the
 *        Delete-One / navigation bookkeeping. Registered in main.cpp as the "record_controller"
 *        context property.
 */
class record_controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool is_recording READ is_recording NOTIFY recording_changed)
    Q_PROPERTY(bool has_result READ has_result NOTIFY result_changed)
    // Live view of the current in-progress record for the status panel: every logged event's
    // value in the exact "{page_id:value}" form it will be written to the Excel Events column,
    // plus the current Result-column value.
    Q_PROPERTY(QStringList logged_events READ logged_events NOTIFY events_changed)
    Q_PROPERTY(QString current_result READ current_result NOTIFY result_changed)

private:
    record current_record;
    bool recording = false;

    // Mirror of the values pushed into current_record.events (same "{page_id:value}" strings),
    // kept here so the QStringList property can be served without adding a getter to `record`.
    QStringList logged_event_values;

    // How many events delete_last_event() can still pop.
    int logged_event_count = 0;

    // Parallel to logged_event_values: for each event, the navigation state from just before the
    // page move it caused (navigation_controller::navigation_state(), set by
    // mark_last_event_navigated), or empty if it caused none. Delete One hands the popped entry to
    // navigation_controller::restore_navigation_state(), so page moves no logged event caused
    // (PageLoader's Back, resume, ...) are never mistaken for one.
    QList<QStringList> logged_event_nav_states;

    /**
     * @brief Pack the four id components into one EVENT_ID_LENGTH(10) string:
     *        page_id(3) + ui_id(3) + ui_#(2) + ui_action(2).
     * @param page_id    3-char page id
     * @param ui_id      3-char control-type mnemonic
     * @param ui_num     2-char per-(page, ui_id) instance index
     * @param ui_action  2-char action code
     * @return the concatenated 10-char id.
     */
    static std::string make_event_id(const QString& page_id, const QString& ui_id, const QString& ui_num, const QString& ui_action);

public:
    explicit record_controller(QObject* parent = nullptr);
    ~record_controller();

    /** @return whether event capture is currently on. */
    bool is_recording() const;
    /** @return whether a bug-report verdict has been submitted (this is what gates Export). */
    bool has_result();
    /** @return every logged event's "{page_id:value}" string, in log order. */
    QStringList logged_events() const;
    /** @return the current verdict's value string, or "" if none set. */
    QString current_result();

    /**
     * @brief Log one event from UI tied to the current page (e.g. an on-screen button). No-op while
     *        recording is off. Stores the value as "{page_id:value}".
     * @param page_id    the page's own id
     * @param ui_id      control-type mnemonic
     * @param ui_num     per-(page, ui_id) instance index
     * @param ui_action  action code ("00", or a state code for toggles)
     * @param value      the control's label plus any state suffix the control appended
     */
    Q_INVOKABLE void record_log_from_page_ui(const QString& page_id, const QString& ui_id, const QString& ui_num, const QString& ui_action, const QString& value);

    /**
     * @brief Handle a press from page-independent UI. Branches on the id's ui_id slot (id.mid(3,3)):
     *        "RES" submits the bug-report verdict (PASS/FAIL) and is the only thing that flips
     *        has_result; any other ui_id logs a page-independent event, gated by is_recording.
     * @param id     already-packed 10-char id (page_id slot is "000")
     * @param value  inner label; stored as "{000:value}"
     */
    Q_INVOKABLE void record_log_from_meta_ui(const QString& id, const QString& value);

    /**
     * @brief Finalize the session: assemble the record, clear the in-progress state and its
     *        bookkeeping, and drop out of recording.
     * @return the completed record_t, to be handed to excel_io_controller::add_record().
     */
    Q_INVOKABLE record_t record_submit();

    /** @brief Flip the recording gate. Navigation on start/stop lives in RecordToggleButton.qml. */
    Q_INVOKABLE void toggle_recording();

    /**
     * @brief Discard the in-progress record and reset its bookkeeping, without exporting, and drop
     *        out of recording (same end state as record_submit()).
     */
    Q_INVOKABLE void clear_record();

    /**
     * @brief Remove the most recently logged event.
     * @return the navigation state from before the page move that event caused, for the caller
     *         (DeleteOneButton.qml) to hand to navigation_controller::restore_navigation_state();
     *         empty if it caused no navigation or nothing was deleted.
     */
    Q_INVOKABLE QStringList delete_last_event();

    /**
     * @brief Tag the most recently logged event as the one that moved pages away from
     *        previous_state. Called right after the click that logged it: on
     *        navigation_controller::navigated_forward (NavButton, wired in DeleteOneButton.qml), and
     *        by BackButton.qml / HomeButton.qml themselves. No-op while recording is off (that
     *        click logged nothing).
     * @param previous_state  navigation_controller::navigation_state() from before the move
     */
    Q_INVOKABLE void mark_last_event_navigated(const QStringList& previous_state);

signals:
    void recording_changed();
    void result_changed();
    void events_changed();
};
