#include "page_registry.hpp"
#include "json/json.h"

#include <QFile>
#include <QByteArray>
#include <QIODevice>

#include <iostream>
#include <memory>

void page_registry::load(std::string& path)
{
    QFile file(QString::fromStdString(path));

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        std::cerr << "page_registry: could not open page config: " << path << std::endl;
        return;
    }

    QByteArray raw = file.readAll();
    file.close();

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errs;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    const char* begin = raw.constData();
    const char* end = begin + raw.size();

    if(!reader->parse(begin, end, &root, &errs)){
        std::cerr << "page_registry: failed to parse page config: " << errs << std::endl;
        return;
    }

    this->page_types.clear();
    this->initial_page_id = root.get("initial_page_id", "").asString();

    const Json::Value& pages = root["pages"];
    for(const auto& page : pages){
        std::string page_id = page.get("page_id", "").asString();
        std::string qml_type = page.get("qml_type", "").asString();

        if(page_id.empty() || qml_type.empty()){
            continue;
        }

        this->page_types[page_id] = qml_type;
    }

    return;
}

std::string page_registry::qml_type_for(const std::string& page_id) const
{
    auto it = this->page_types.find(page_id);
    if(it == this->page_types.end()){
        return std::string();
    }

    return it->second;
}

std::string page_registry::get_initial_page_id() const
{
    return this->initial_page_id;
}

page_registry::~page_registry()
{
}
