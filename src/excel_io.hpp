/**
 * @file excel_io.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief Providing CRUD functions for excel file
 * @version 0.1
 * @date 2026-08-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "common.hpp"

/**
 * @brief An in-memory list of bug reports (`std::vector<record_t>`) backed by an .xlsx file, using
 *        the fixed worksheet layout from common.hpp. Qt-free; excel_io_controller is the QObject
 *        wrapper. load()/create()/export_records() are the file I/O; read/push/update/pop/delete
 *        operate only on the in-memory list.
 */
class excel_io
{
private:
    std::vector<record_t> records;
    std::string file_path;

public:

    /**
     * @brief Read records from the .xlsx at `path` into memory, replacing the current list.
     *        Rejects a file that fails the format check (no DEFAULT_COUNT_LABEL in A1) or throws
     *        while being read; on rejection the current records are left untouched and the stored
     *        path is reset to empty.
     * @param path  filesystem path to the .xlsx (read only; not modified).
     * @return true on success, false if the file is missing or not a recognized bug-report file.
     */
    bool load(std::string& path);

    /**
     * @brief Create a new empty bug-report .xlsx at `path` (format markers + A2:D2 header row),
     *        then load() it so this object is ready to use.
     * @param path  filesystem path to create (read only; not modified).
     */
    void create(std::string& path);

    /** @return whether a file has been loaded/created (i.e. a path is set). */
    bool has_file() const;

    /**
     * @param index  0-based record index.
     * @return the record at `index`, or a default-constructed record_t if `index` is out of range.
     */
    record_t read_record(unsigned int index);

    /**
     * @brief Append a record to the in-memory list (does not write the file).
     * @param record  record to append; copied in, not modified.
     */
    void push_record(record_t& record);

    /**
     * @brief Replace the record at `index`. No-op if `index` is out of range.
     * @param index   0-based record index.
     * @param record  replacement record; copied in, not modified.
     */
    void update_record(unsigned int index, record_t& record);

    /**
     * @brief Remove and return the last record.
     * @return the removed record, or a default-constructed record_t if the list is empty.
     */
    record_t pop_record();

    /**
     * @brief Remove and return the record at `index`. No-op (returns empty) if out of range.
     * @param index  0-based record index.
     * @return the removed record, or a default-constructed record_t.
     */
    record_t delete_record(unsigned int index);

    /**
     * @brief Write the in-memory records to the backing file, opening it if it exists or creating
     *        it otherwise: refreshes the A1 label + header row + B1 count, writes one row per
     *        record from DEFAULT_TABLE_RAW_START, and clears any rows left over from a larger
     *        previous export. No-op (with a warning) if no path is set.
     */
    void export_records();

    excel_io(){
        this->records = std::vector<record_t>();
        file_path = std::string();
    }

    ~excel_io();
};
