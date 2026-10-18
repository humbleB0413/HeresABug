#include "record_controller.hpp"

record_controller::record_controller(QObject* parent)
    : QObject(parent)
{
}

record_controller::~record_controller()
{
}

std::string record_controller::make_event_id(const QString& page_id, const QString& ui_id, const QString& ui_num, const QString& ui_action)
{
    return (page_id + ui_id + ui_num + ui_action).toStdString();
}

bool record_controller::is_recording() const
{
    return this->recording;
}

bool record_controller::has_result()
{
    return !this->current_record.pop_result().id.empty();
}

QStringList record_controller::logged_events() const
{
    return this->logged_event_values;
}

QString record_controller::current_result()
{
    return QString::fromStdString(this->current_record.pop_result().value);
}

void record_controller::record_log_from_page_ui(const QString& page_id, const QString& ui_id, const QString& ui_num, const QString& ui_action, const QString& value)
{
    if(!this->recording){
        return;
    }

    // Every logged value is stored (and later exported) in "{page_id:value}" form; `value` is the
    // caller's default label plus, for state-varying controls, a trailing " On"/typed-text/etc.
    QString formatted = "{" + page_id + ":" + value + "}";

    event_t event;
    event.id = make_event_id(page_id, ui_id, ui_num, ui_action);
    event.value = formatted.toStdString();

    this->current_record.push_event(event);
    this->logged_event_count++;
    this->logged_event_values.append(formatted);
    this->logged_event_nav_states.append(QStringList());
    emit events_changed();
    return;
}

void record_controller::record_log_from_meta_ui(const QString& id, const QString& value)
{
    // Same "{page_id:value}" form; the meta id's first 3 chars are its page_id slot ("000").
    QString formatted = "{" + id.left(3) + ":" + value + "}";

    // The meta id is a packed EVENT_ID_LENGTH string: page_id(3) + ui_id(3) + ui_#(2) + ui_action(2).
    // Only ui_id "RES" (the PASS/FAIL bug-report verdict) submits the record's single result — that
    // is the flag ExportButton waits on, so nothing else may set it. Every other page-independent
    // meta control (device hard keys, etc.) logs an ordinary event instead.
    const bool is_result = (id.mid(3, 3) == "RES");

    if(is_result){
        event_t result;
        result.id = id.toStdString();
        result.value = formatted.toStdString();

        this->current_record.push_result(result);
        emit result_changed();
        return;
    }

    // Non-verdict meta control: a page-independent event. Gated by `recording` and mirrored into the
    // live status panel / Delete One stack exactly like record_log_from_page_ui.
    if(!this->recording){
        return;
    }

    event_t event;
    event.id = id.toStdString();
    event.value = formatted.toStdString();

    this->current_record.push_event(event);
    this->logged_event_count++;
    this->logged_event_values.append(formatted);
    this->logged_event_nav_states.append(QStringList());
    emit events_changed();
    return;
}

record_t record_controller::record_submit()
{
    record_t result = this->current_record.export_record();
    this->logged_event_count = 0;
    this->logged_event_values.clear();
    this->logged_event_nav_states.clear();

    // Submitting is the end of a session: drop back out of recording so the app is in the same
    // state it starts in (idle, empty record). The caller (ExportButton) additionally sends the
    // navigation back to the initial page.
    if(this->recording){
        this->recording = false;
        emit recording_changed();
    }

    emit result_changed();
    emit events_changed();
    return result;
}

void record_controller::toggle_recording()
{
    this->recording = !this->recording;
    emit recording_changed();
    return;
}

void record_controller::clear_record()
{
    this->current_record.clear();
    this->logged_event_count = 0;
    this->logged_event_values.clear();
    this->logged_event_nav_states.clear();

    // Clearing discards the session, so like record_submit() it drops back out of recording. The
    // caller (ClearRecordButton) additionally resets navigation to the initial page.
    if(this->recording){
        this->recording = false;
        emit recording_changed();
    }

    emit result_changed();
    emit events_changed();
    return;
}

QStringList record_controller::delete_last_event()
{
    if(this->logged_event_count <= 0){
        return QStringList();
    }

    this->current_record.pop_event();
    this->logged_event_count--;
    if(!this->logged_event_values.isEmpty()){
        this->logged_event_values.removeLast();
    }
    QStringList nav_state;
    if(!this->logged_event_nav_states.isEmpty()){
        nav_state = this->logged_event_nav_states.takeLast();
    }
    emit events_changed();

    return nav_state;
}

void record_controller::mark_last_event_navigated(const QStringList& previous_state)
{
    if(!this->recording || this->logged_event_nav_states.isEmpty()){
        return;
    }

    this->logged_event_nav_states.last() = previous_state;
    return;
}
