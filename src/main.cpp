#include "main.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    record_controller record_controller_instance;
    navigation_controller navigation_controller_instance;
    excel_io_controller excel_io_controller_instance;

    navigation_controller_instance.load_page_config(":/pages.json");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("record_controller", &record_controller_instance);
    engine.rootContext()->setContextProperty("navigation_controller", &navigation_controller_instance);
    engine.rootContext()->setContextProperty("excel_io_controller", &excel_io_controller_instance);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("HeresABug", "Main");

    return app.exec();
}
