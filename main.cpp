#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include "backend.h"
#include <memory>
#include "mqtt_clock/mqttclock.h"

int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    auto back_end = std::make_shared<backend>();

    const QHostInfo ipAddr = QHostInfo::fromName(back_end->ipAddr());
    const QHostAddress EXAMPLE_HOST = ipAddr.addresses()[0];
    const quint16 EXAMPLE_PORT = back_end->portAddr().toUInt();
    const QString EXAMPLE_TOPIC = "/clock/time";
    QStringList topics;
    topics << "/clock/time" << "/clock/alarm" << "/clock/prayer/time" << "/clock/prayer/name";
    MqttClock mqttClock(topics, EXAMPLE_HOST, EXAMPLE_PORT, back_end);
    mqttClock.connectToHost();




    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    engine.rootContext()->setContextProperty("backend", back_end.get());
    engine.load(url);

    return app.exec();
}
