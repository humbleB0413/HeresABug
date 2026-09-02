/**
 * @file record.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief Recording a single bug report(Serial events and One result)
 * @version 0.1
 * @date 2026-08-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "common.hpp"


/**
 * @brief One in-progress bug report: an ordered list of UI events plus a single outcome.
 *
 * `events` is a std::deque, but push_event()/pop_event() both operate on the back, so it behaves
 * as a LIFO stack (the most recent event is the one that pops). Qt-free and independently testable;
 * record_controller is the QObject wrapper that exposes it to QML.
 */
class record
{
private:
    std::deque<event_t> events;
    event_t result;

public:
    record(){
        events = std::deque<event_t>();
        result.id = std::string();
        result.value = std::string();
    }

    /**
     * @brief Append an event to the back of the event list (the stack top).
     * @param event  event to store; copied in, not modified.
     */
    void push_event(event_t& event);

    /**
     * @brief Remove and return the most recently pushed event.
     * @return the removed event, or a default-constructed event_t if the list is empty.
     */
    event_t pop_event();

    /**
     * @brief Set the record's single outcome, replacing any previous one.
     * @param result  outcome event (packed id + value); copied in, not modified.
     */
    void push_result(event_t& result);

    /**
     * @brief Read the current outcome WITHOUT clearing it (a peek, despite the name).
     * @return the outcome event; default-constructed if none has been set.
     */
    event_t pop_result();

    /**
     * @brief Drop all events and the outcome, returning the record to its empty state.
     */
    void clear();

    /**
     * @brief Assemble the finished record and reset this object.
     *
     * Builds record_t.id by concatenating every event's id followed by the result id, moves the
     * events and result into the returned value, then clear()s internal state as a side effect
     * (a second call returns an empty record).
     * @return the assembled record_t.
     */
    record_t export_record();

    ~record();
};
