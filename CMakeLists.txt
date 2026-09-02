cmake_minimum_required(VERSION 3.16)

project(HeresABug VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(OpenXLSX CONFIG REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(HeresABug
    src/main.cpp
    src/record.cpp
    src/record_controller.cpp
    src/excel_io.cpp
    src/excel_io_controller.cpp
    src/page_registry.cpp
    src/navigation_controller.cpp
    src/jsoncpp.cpp
)


qt_policy(SET QTP0004 NEW)

# Which qml/<dir> supplies this build's Main.qml (the module's "Main" type, loaded via
# engine.loadFromModule) plus its pages.json and page .qml files. qml/common/ is always built in
# regardless — it holds the reusable building blocks every app dir composes, not app-specific
# content. Switch projects with: cmake --preset qt-mingw -DHERESABUG_APP_DIR=example_2
set(HERESABUG_APP_DIR "example_from_scratch" CACHE STRING "qml subfolder providing this build's Main.qml/pages.json (e.g. example, example_2)")

# Every .qml file under qml/common/ and qml/${HERESABUG_APP_DIR}/ gets picked up automatically,
# so adding a new common component or app page never needs a CMakeLists.txt edit.
file(GLOB_RECURSE COMMON_QML_FILES CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/qml/common/*.qml")
file(GLOB_RECURSE APP_QML_FILES CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/qml/${HERESABUG_APP_DIR}/*.qml")

qt_add_qml_module(HeresABug
    URI HeresABug
    VERSION 1.0
    QML_FILES
        ${COMMON_QML_FILES}
        ${APP_QML_FILES}
)

# BASE strips "qml/${HERESABUG_APP_DIR}" from the resource path, so this always lands at the fixed
# runtime path ":/pages.json" no matter which app dir is selected — main.cpp doesn't need to know
# HERESABUG_APP_DIR's value. Every app dir must have a pages.json (even an empty "{}" is fine —
# page_registry::load() handles an empty/pages-less config gracefully at runtime); qt_add_resources
# requires FILES to exist on disk at configure time, so a missing one fails the build on purpose.
qt_add_resources(HeresABug "page_config"
    PREFIX "/"
    BASE "qml/${HERESABUG_APP_DIR}"
    FILES "qml/${HERESABUG_APP_DIR}/pages.json"
)

# Pin the Quick Controls style. Qt auto-reads ":/qtquickcontrols2.conf" at startup. The default on
# Windows is the native style, which (a) can't host the custom Canvas contentItem the record-control
# buttons use — their icons silently don't render — and (b) was the source of a segfault during app
# teardown. "Basic" fixes both.
qt_add_resources(HeresABug "controls_conf"
    PREFIX "/"
    FILES "qtquickcontrols2.conf"
)

target_link_libraries(HeresABug
    PRIVATE Qt6::Quick
    PRIVATE OpenXLSX::OpenXLSX
)

install(TARGETS HeresABug
    BUNDLE DESTINATION .
    RUNTIME DESTINATION .
)
