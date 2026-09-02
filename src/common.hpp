/**
 * @file common.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief Common components that every source file uses
 * @version 0.1
 * @date 2026-08-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <string>
#include <deque>
#include <vector>

#define DEFAULT_WORKSHEET "Sheet1"
#define DEFAULT_COUNT_LABEL_CELL "A1"
#define DEFAULT_COUNT_LABEL "Total Counts"
#define DEFAULT_COUNT_INFO "B1"
#define DEFAULT_TABLE_HEADER_ROW 2
#define DEFAULT_HEADER_ID "Bug Report ID"
#define DEFAULT_HEADER_EVENTS "Events"
#define DEFAULT_HEADER_RESULTS "Results"
#define DEFAULT_HEADER_NOTE "Note"
#define DEFAULT_TABLE_RAW_START 3
#define TABLE_COL_SIZE 4
#define EVENTS_PARSER ", "
#define EVENT_ID_LENGTH 10// page_id(3), ui_id(3), ui_#(2), ui_action(2)

/**
 * @brief One logged action. @c id is the packed EVENT_ID_LENGTH id; @c value is the
 *        "{page_id:value}" label string that ends up in the Excel Events/Results cell.
 */
typedef struct {
    std::string id;
    std::string value;
} event_t;

/**
 * @brief A finished bug report ready for a spreadsheet row. @c id is every event id concatenated
 *        followed by the result id (column A); @c events are the individual events (column B,
 *        joined by EVENTS_PARSER); @c result is the single outcome (column C).
 */
typedef struct{
    std::string id;
    std::deque<event_t> events;
    event_t result;
} record_t;