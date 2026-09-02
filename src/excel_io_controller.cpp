#include "excel_io_controller.hpp"

excel_io_controller::excel_io_controller(QObject* parent)
    : QObject(parent)
{
}

excel_io_controller::~excel_io_controller()
{
}

bool excel_io_controller::file_ready() const
{
    return this->io.has_file();
}

bool excel_io_controller::load_file(const QUrl& path)
{
    std::string path_str = path.toLocalFile().toStdString();
    bool ok = this->io.load(path_str);
    emit file_ready_changed();
    return ok;
}

void excel_io_controller::create_file(const QUrl& path)
{
    std::string path_str = path.toLocalFile().toStdString();
    this->io.create(path_str);
    emit file_ready_changed();
    return;
}

record_t excel_io_controller::read_record(int index)
{
    return this->io.read_record(static_cast<unsigned int>(index));
}

void excel_io_controller::add_record(const record_t& record)
{
    record_t record_copy = record;
    this->io.push_record(record_copy);
    return;
}

void excel_io_controller::update_record(int index, const record_t& record)
{
    record_t record_copy = record;
    this->io.update_record(static_cast<unsigned int>(index), record_copy);
    return;
}

record_t excel_io_controller::delete_record(int index)
{
    return this->io.delete_record(static_cast<unsigned int>(index));
}

void excel_io_controller::export_records()
{
    this->io.export_records();
    return;
}
