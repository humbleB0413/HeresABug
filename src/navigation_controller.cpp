#include "navigation_controller.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

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
    QStringList previous_state = this->navigation_state();

    if(!this->switch_to(id)){
        return;
    }

    if(!previous_id.empty()){
        this->history.push_back(previous_id);
        emit this->navigated_forward(previous_state);
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

    // Unwind, not stack, whatever navigation happened while recording was stopped.
    this->unwind_to(this->recording_anchor_id);
    return;
}

void navigation_controller::unwind_to(const std::string& page_id)
{
    // Pop the back-stack until page_id is current again.
    while(this->current_id != page_id && !this->history.empty()){
        std::string previous_id = this->history.back();
        this->history.pop_back();
        if(!this->switch_to(previous_id)){
            break;
        }
    }

    // page_id wasn't behind us in history (e.g. the user went back past it, or navigated backwards
    // while stopped): land on it directly, still without pushing a new history entry.
    if(this->current_id != page_id){
        this->switch_to(page_id);
    }

    return;
}

bool navigation_controller::go_back_to(const QStringList& page_ids)
{
    std::vector<std::string> ids;
    for(const QString& page_id : page_ids){
        std::string id = page_id.toStdString();
        if(this->pages.qml_type_for(id).empty()){
            std::cerr << "navigation_controller: unknown page_id " << id << std::endl;
            continue;
        }
        ids.push_back(id);
    }

    if(ids.empty()){
        std::cerr << "navigation_controller: no target page to go back to" << std::endl;
        return false;
    }

    // Already on one of the targets: nothing to pop.
    if(std::find(ids.begin(), ids.end(), this->current_id) != ids.end()){
        return false;
    }

    // Drop the most recent history entry that is any of the targets, and everything above it; if
    // none was visited, that is the whole history and the first target is landed on. Switching
    // once (not page by page) keeps PageLoader from building the pages in between.
    auto found = std::find_first_of(this->history.rbegin(), this->history.rend(), ids.begin(), ids.end());
    std::string target = (found == this->history.rend()) ? ids.front() : *found;
    this->history.erase(found == this->history.rend() ? this->history.begin() : std::prev(found.base()), this->history.end());

    return this->switch_to(target);
}

QStringList navigation_controller::navigation_state() const
{
    QStringList state;
    if(this->current_id.empty()){
        return state;
    }

    for(const std::string& page_id : this->history){
        state.append(QString::fromStdString(page_id));
    }
    state.append(QString::fromStdString(this->current_id));
    return state;
}

void navigation_controller::restore_navigation_state(const QStringList& state)
{
    if(state.isEmpty()){
        return;
    }

    std::string id = state.last().toStdString();
    if(this->pages.qml_type_for(id).empty()){
        std::cerr << "navigation_controller: unknown page_id " << id << std::endl;
        return;
    }

    this->history.clear();
    for(int i = 0; i < state.size() - 1; i++){
        this->history.push_back(state.at(i).toStdString());
    }

    this->switch_to(id);
    return;
}

bool navigation_controller::go_back()
{
    if(this->history.empty()){
        std::cerr << "navigation_controller: no previous page to go back to" << std::endl;
        return false;
    }

    std::string previous_id = this->history.back();
    this->history.pop_back();

    return this->switch_to(previous_id);
}

QString navigation_controller::current_page_type() const
{
    return QString::fromStdString(this->pages.qml_type_for(this->current_id));
}

QString navigation_controller::current_page_id() const
{
    return QString::fromStdString(this->current_id);
}
