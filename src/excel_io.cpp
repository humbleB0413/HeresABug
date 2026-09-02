#include "excel_io.hpp"
#include <filesystem>
#include <iostream>
#include <OpenXLSX.hpp>

/**
 * @brief Cut `str` into fixed-width chunks (the last one may be shorter).
 * @param str         source string
 * @param chunk_size  chunk width
 * @return the chunks in order; empty vector if `str` is empty.
 */
static std::vector<std::string> split_by_length(const std::string& str, size_t chunk_size);

/**
 * @brief Split `str` on every occurrence of `delimiter`.
 * @param str        source string
 * @param delimiter  separator; if empty, `str` is returned as the single token
 * @return the tokens in order (always at least one).
 */
std::vector<std::string> split_by_delimiter(const std::string& str, const std::string& delimiter);

/**
 * @brief Write the A2:D2 column-header row (Bug Report ID / Events / Results / Note) that
 *        reference/REF_bug_report.xlsx keeps in row DEFAULT_TABLE_HEADER_ROW.
 * @param wks  the worksheet to write into.
 */
static void write_table_header(OpenXLSX::XLWorksheet& wks)
{
    wks.cell(DEFAULT_TABLE_HEADER_ROW, 1).value() = DEFAULT_HEADER_ID;
    wks.cell(DEFAULT_TABLE_HEADER_ROW, 2).value() = DEFAULT_HEADER_EVENTS;
    wks.cell(DEFAULT_TABLE_HEADER_ROW, 3).value() = DEFAULT_HEADER_RESULTS;
    wks.cell(DEFAULT_TABLE_HEADER_ROW, 4).value() = DEFAULT_HEADER_NOTE;
}

bool excel_io::load(std::string &path)
{
    this->file_path = path;

    if(!std::filesystem::exists(this->file_path)){
        std::cerr << "There is no file." << std::endl;
        this->file_path = std::string();
        return false;
    }

    OpenXLSX::XLDocument excel;

    try {
        excel.open(this->file_path);

        OpenXLSX::XLWorksheet wks = excel.workbook().worksheet(DEFAULT_WORKSHEET);

        // Format check: a genuine bug-report file always has the DEFAULT_COUNT_LABEL text in
        // DEFAULT_COUNT_LABEL_CELL (see reference/REF_bug_report.xlsx). Reject anything else
        // instead of misreading its DEFAULT_COUNT_INFO cell as a record count.
        OpenXLSX::XLCellValue label = wks.cell(DEFAULT_COUNT_LABEL_CELL).value();
        if(label.type() != OpenXLSX::XLValueType::String || label.get<std::string>() != DEFAULT_COUNT_LABEL){
            std::cerr << "Not a recognized bug-report file (expected \"" << DEFAULT_COUNT_LABEL
                       << "\" in " << DEFAULT_COUNT_LABEL_CELL << ")." << std::endl;
            excel.close();
            this->file_path = std::string();
            return false;
        }

        records.clear(); // initialize, only once the file is confirmed valid

        int table_size = wks.cell(DEFAULT_COUNT_INFO).value().get<int>();

        for(int i = DEFAULT_TABLE_RAW_START; i < DEFAULT_TABLE_RAW_START+table_size; i++){
            std::string record_id = wks.cell(i, 1).value().get<std::string>();
            std::string record_events = wks.cell(i, 2).value().get<std::string>();
            std::string record_result = wks.cell(i, 3).value().get<std::string>();

            std::vector<std::string> parsed_ids = split_by_length(record_id, EVENT_ID_LENGTH);
            std::vector<std::string> parsed_events = split_by_delimiter(record_events, EVENTS_PARSER);

            // A well-formed id is (event ids...) + (result id), so at least one chunk. Skip a row
            // whose id cell is empty/garbage rather than indexing past the end of parsed_ids.
            if(parsed_ids.empty()){
                std::cerr << "excel_io::load: row " << i << " has no Bug Report ID, skipping" << std::endl;
                continue;
            }

            record_t record;
            record.id = record_id;
            record.result.id = parsed_ids.back();
            record.result.value = record_result;

            // event count = min(events cell tokens, id chunks minus the trailing result id)
            size_t event_count = parsed_events.size();
            if(event_count > parsed_ids.size() - 1){
                event_count = parsed_ids.size() - 1;
            }
            for(size_t j = 0; j < event_count; j++){
                event_t event = {parsed_ids[j], parsed_events[j]};
                record.events.push_back(event);
            }

            this->records.push_back(record);
        }

        excel.close();
    }
    catch(const OpenXLSX::XLException&){
        std::cerr << "Not a recognized bug-report file." << std::endl;
        this->file_path = std::string();
        return false;
    }

    return true;
}

void excel_io::create(std::string &path)
{
    this->file_path = path;
    OpenXLSX::XLDocument excel;
    excel.create(this->file_path);
    OpenXLSX::XLWorksheet wks = excel.workbook().worksheet(DEFAULT_WORKSHEET);
    wks.cell(DEFAULT_COUNT_LABEL_CELL).value() = DEFAULT_COUNT_LABEL;
    wks.cell(DEFAULT_COUNT_INFO).value() = 0;
    write_table_header(wks);
    excel.save();
    excel.close();
    this->load(this->file_path);
}

record_t excel_io::read_record(unsigned int index)
{
    if(this->records.empty() || index >= records.size()){
        return record_t();
    }

    return this->records[index];
}

void excel_io::push_record(record_t &record)
{
    this->records.push_back(record);
    return;
}

void excel_io::update_record(unsigned int index, record_t &record)
{
    if(this->records.empty() || index >= records.size()){
        return;
    }

    this->records[index] = record;
    return;
}

record_t excel_io::pop_record()
{
    if(this->records.empty()){
        return record_t();
    }

    record_t result = this->records.back();
    this->records.pop_back();
    return result;
}

record_t excel_io::delete_record(unsigned int index)
{
    if(this->records.empty() || index >= records.size()){
        return record_t();
    }

    record_t result = this->records[index];
    this->records.erase(records.begin() + index);
    return result;
}

void excel_io::export_records()
{
    if(this->file_path.empty()){
        std::cerr << "There is no file path set." << std::endl;
        return;
    }

    OpenXLSX::XLDocument excel;

    if(std::filesystem::exists(this->file_path)){
        excel.open(this->file_path);
    }
    else{
        excel.create(this->file_path);
    }

    OpenXLSX::XLWorksheet wks = excel.workbook().worksheet(DEFAULT_WORKSHEET);

    int old_count = 0;
    if(wks.cell(DEFAULT_COUNT_INFO).value().type() == OpenXLSX::XLValueType::Integer){
        old_count = wks.cell(DEFAULT_COUNT_INFO).value().get<int>();
    }

    // Keep an exported file self-consistent with reference/REF_bug_report.xlsx even when
    // export_records() had to create it from scratch (the format markers load() checks + the
    // column-header row). Harmless overwrite when the file already had them.
    wks.cell(DEFAULT_COUNT_LABEL_CELL).value() = DEFAULT_COUNT_LABEL;
    write_table_header(wks);

    int new_count = static_cast<int>(this->records.size());
    wks.cell(DEFAULT_COUNT_INFO).value() = new_count;

    for(int i = 0; i < new_count; i++){
        const record_t& record = this->records[i];
        int row = DEFAULT_TABLE_RAW_START + i;

        std::string events_str;
        for(size_t j = 0; j < record.events.size(); j++){
            if(j > 0){
                events_str += EVENTS_PARSER;
            }
            events_str += record.events[j].value;
        }

        wks.cell(row, 1).value() = record.id;
        wks.cell(row, 2).value() = events_str;
        wks.cell(row, 3).value() = record.result.value;
    }

    // clear any leftover rows from a previous export that had more records than this one
    for(int i = new_count; i < old_count; i++){
        int row = DEFAULT_TABLE_RAW_START + i;
        for(int col = 1; col <= TABLE_COL_SIZE; col++){
            wks.cell(row, col).value().clear();
        }
    }

    excel.save();
    excel.close();
}

bool excel_io::has_file() const
{
    return !this->file_path.empty();
}

static std::vector<std::string> split_by_length(const std::string& str, size_t chunk_size) {
    std::vector<std::string> result;
    for (size_t i = 0; i < str.size(); i += chunk_size) {
        result.push_back(str.substr(i, chunk_size));
    }
    return result;
}

std::vector<std::string> split_by_delimiter(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;

    if (delimiter.empty()) {
        result.push_back(str);
        return result;
    }

    size_t start_pos = 0;
    size_t found_pos = 0;

    while ((found_pos = str.find(delimiter, start_pos)) != std::string::npos) {
        result.push_back(str.substr(start_pos, found_pos - start_pos));
        start_pos = found_pos + delimiter.length(); // skip past the delimiter
    }
    result.push_back(str.substr(start_pos)); // trailing token

    return result;
}

excel_io::~excel_io()
{
}
