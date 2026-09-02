/**
 * @file page_registry.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief Loads the page_id -> qml type name mapping used for page navigation from a JSON config file
 * @version 0.1
 * @date 2026-08-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include <string>
#include <unordered_map>

/**
 * @brief Parses a page-navigation config (pages.json) into a page_id -> QML-type-name map plus an
 *        initial page id. Qt-UI-free (uses jsoncpp + QFile for resource reads); navigation_controller
 *        owns an instance and does the actual page switching.
 */
class page_registry
{
private:
    std::unordered_map<std::string, std::string> page_types;
    std::string initial_page_id;

public:
    /**
     * @brief Read and parse the JSON config, filling the page map and initial page id. On an
     *        open or parse failure the map is left empty and a message is written to stderr
     *        (callers stay usable; navigation just finds no pages).
     * @param path  filesystem/resource path to the JSON file (read only; not modified).
     */
    void load(std::string& path);

    /**
     * @param page_id  the page id to look up.
     * @return the registered QML type name, or "" if `page_id` is unknown.
     */
    std::string qml_type_for(const std::string& page_id) const;

    /** @return the configured initial page id, or "" if the config set none. */
    std::string get_initial_page_id() const;

    page_registry(){
        page_types = std::unordered_map<std::string, std::string>();
        initial_page_id = std::string();
    }

    ~page_registry();
};
