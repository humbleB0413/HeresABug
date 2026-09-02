#include "record.hpp"
#include <algorithm>

void record::push_event(event_t& event)
{
    this->events.push_back(event);
    return;
}

event_t record::pop_event()
{
    if(this->events.empty()){
        return event_t();
    }

    event_t result = this->events.back();
    this->events.pop_back();

    return result;
}

void record::push_result(event_t& result)
{
    this->result = result;
    return;
}

event_t record::pop_result()
{
    return this->result;
}

void record::clear()
{
    this->events = std::deque<event_t>();
    this->result = event_t();
    return;
}

record::~record()
{
}

record_t record::export_record()
{   
    record_t result;
    result.id = "";
    result.events = this->events;
    result.result = this->result;
    
    for(auto event : result.events){
        result.id += event.id;
    }
    result.id += result.result.id;

    this->clear();

    return result;
}
