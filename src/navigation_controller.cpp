#include "navigation_controller.hpp"

#include <iostream>

navigation_controller::navigation_controller(QObject* parent)
    : QObject(parent)
{
}

navigation_controller::~navigation_controller()
{
}

void navigation_controller::load_page_config(const QString& path)
{
    std::string path_str = path.toStdString();
    this->pages.load(path_str);

    std::string initial_id = this->pages.get_initial_page_id();
    if(!initial_id.empty()){
        this->go_to_page(QString::fromStdString(initial_id));
    }

    return;
}

bool navigation_controller::switch_to(const std::string& page_id)
{
    if(this->pages.qml_type_for(page_id).empty()){
        std::cerr << "navigation_controller: unknown page_id " << page_id << std::endl;
        return false;
    }

    this->current_id = page_id;
    emit this->current_page_changed();
    return true;
}

void navigation_controller::go_to_page(const QString& page_id)
{
    std::string id = page_id.toStdString();
    std::string previous_id = this->current_id;

    if(!this->switch_to(id)){
        return;
    }

    if(!previous_id.empty()){
        this->history.push_back(previous_id);
    }

    return;
}

void navigation_controller::go_to_initial_page()
{
    std::string initial_id = this->pages.get_initial_page_id();
    if(initial_id.empty()){
        std::cerr << "navigation_controller: no initial page configured" << std::endl;
        return;
    }

    this->history.clear();
    this->recording_anchor_id.clear();
    // Emitted before the page switch so PageLoader drops its cached pages first and the initial
    // page is rebuilt fresh, with no local state left over from the previous session.
    emit this->session_reset();
    this->switch_to(initial_id);
    return;
}

void navigation_controller::mark_recording_anchor()
{
    this->recording_anchor_id = this->current_id;
    return;
}

void navigation_controller::resume_to_recording_anchor()
{
    // No anchor yet (first session, or one cleared by go_to_initial_page): keep the original
    // "every session begins at page one" behaviour.
    if(this->recording_anchor_id.empty() || this->pages.qml_type_for(this->recording_anchor_id).empty()){
        this->go_to_initial_page();
        return;
    }

    // Pop the back-stack until the stop page is current again — unwinding, not stacking, whatever
    // navigation happened while recording was stopped.
    while(this->current_id != this->recording_anchor_id && !this->history.empty()){
        std::string previous_id = this->history.back();
        this->history.pop_back();
        if(!this->switch_to(previous_id)){
            break;
        }
    }

    // Anchor wasn't behind us in history (e.g. the user navigated backwards while stopped): land on
    // it directly, still without pushing a new history entry.
    if(this->current_id != this->recording_anchor_id){
        this->switch_to(this->recording_anchor_id);
    }

    return;
}

void navigation_controller::go_back()
{
    if(this->history.empty()){
        std::cerr << "navigation_controller: no previous page to go back to" << std::endl;
        return;
    }

    std::string previous_id = this->history.back();
    this->history.pop_back();

    this->switch_to(previous_id);
    return;
}

QString navigation_controller::current_page_type() const
{
    return QString::fromStdString(this->pages.qml_type_for(this->current_id));
}

QString navigation_controller::current_page_id() const
{
    return QString::fromStdString(this->current_id);
}
