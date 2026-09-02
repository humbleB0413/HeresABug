/**
 * @file excel_io_controller.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief QObject wrapper exposing `excel_io`'s CRUD functions to QML
 * @version 0.1
 * @date 2026-08-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "common.hpp"
#include "excel_io.hpp"

#include <QObject>
#include <QString>
#include <QUrl>

/**
 * @brief QML-facing adapter around one `excel_io`. Converts the QUrl the file dialogs hand back
 *        into a local path, forwards CRUD to `io`, and exposes file_ready so buttons can enable
 *        themselves. Registered in main.cpp as the "excel_io_controller" context property.
 */
class excel_io_controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool file_ready READ file_ready NOTIFY file_ready_changed)

private:
    excel_io io;

public:
    explicit excel_io_controller(QObject* parent = nullptr);
    ~excel_io_controller();

    /** @return whether a destination file has been created/loaded. */
    bool file_ready() const;

    /**
     * @brief Load an existing .xlsx (QtQuick.Dialogs OpenFile). Emits file_ready_changed.
     * @param path  file URL from FileDialog.selectedFile.
     * @return true if accepted, false if rejected for wrong format.
     */
    Q_INVOKABLE bool load_file(const QUrl& path);

    /**
     * @brief Create a new .xlsx (QtQuick.Dialogs SaveFile) and load it. Emits file_ready_changed.
     * @param path  file URL from FileDialog.selectedFile.
     */
    Q_INVOKABLE void create_file(const QUrl& path);

    /**
     * @param index  0-based record index.
     * @return the record at `index`, or an empty record_t if out of range.
     */
    Q_INVOKABLE record_t read_record(int index);

    /**
     * @brief Append a finished record to the in-memory list (e.g. record_controller::record_submit()'s
     *        return value). Call export_records() afterwards to persist.
     * @param record  the record to append.
     */
    Q_INVOKABLE void add_record(const record_t& record);

    /**
     * @brief Replace the record at `index`. No-op if out of range.
     * @param index   0-based record index.
     * @param record  replacement record.
     */
    Q_INVOKABLE void update_record(int index, const record_t& record);

    /**
     * @brief Remove and return the record at `index`.
     * @param index  0-based record index.
     * @return the removed record, or an empty record_t if out of range.
     */
    Q_INVOKABLE record_t delete_record(int index);

    /** @brief Write all in-memory records to the backing .xlsx. */
    Q_INVOKABLE void export_records();

signals:
    void file_ready_changed();
};
