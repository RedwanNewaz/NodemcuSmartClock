QT += quick qml core network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/qmqtt/src/mqtt

SOURCES += \
        backend.cpp \
        main.cpp \
        mqtt_clock/mqttclock.cpp \
        qmqtt/src/mqtt/qmqtt_client.cpp \
        qmqtt/src/mqtt/qmqtt_client_p.cpp \
        qmqtt/src/mqtt/qmqtt_frame.cpp \
        qmqtt/src/mqtt/qmqtt_message.cpp \
        qmqtt/src/mqtt/qmqtt_network.cpp \
        qmqtt/src/mqtt/qmqtt_router.cpp \
        qmqtt/src/mqtt/qmqtt_routesubscription.cpp \
        qmqtt/src/mqtt/qmqtt_socket.cpp \
        qmqtt/src/mqtt/qmqtt_ssl_socket.cpp \
        qmqtt/src/mqtt/qmqtt_timer.cpp \
        qmqtt/src/mqtt/qmqtt_websocket.cpp \
        qmqtt/src/mqtt/qmqtt_websocketiodevice.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    qmqtt/src/mqtt/qmqtt.pri \
    qmqtt/src/mqtt/qmqtt.qbs \
    resources/backend.png \
    resources/backend.svg

HEADERS += \
    backend.h \
    mqtt_clock/mqttclock.h \
    qmqtt/src/mqtt/qmqtt.h \
    qmqtt/src/mqtt/qmqtt_client.h \
    qmqtt/src/mqtt/qmqtt_client_p.h \
    qmqtt/src/mqtt/qmqtt_frame.h \
    qmqtt/src/mqtt/qmqtt_global.h \
    qmqtt/src/mqtt/qmqtt_message.h \
    qmqtt/src/mqtt/qmqtt_message_p.h \
    qmqtt/src/mqtt/qmqtt_network_p.h \
    qmqtt/src/mqtt/qmqtt_networkinterface.h \
    qmqtt/src/mqtt/qmqtt_routedmessage.h \
    qmqtt/src/mqtt/qmqtt_router.h \
    qmqtt/src/mqtt/qmqtt_routesubscription.h \
    qmqtt/src/mqtt/qmqtt_socket_p.h \
    qmqtt/src/mqtt/qmqtt_socketinterface.h \
    qmqtt/src/mqtt/qmqtt_ssl_socket_p.h \
    qmqtt/src/mqtt/qmqtt_timer_p.h \
    qmqtt/src/mqtt/qmqtt_timerinterface.h \
    qmqtt/src/mqtt/qmqtt_websocket_p.h \
    qmqtt/src/mqtt/qmqtt_websocketiodevice_p.h

SUBDIRS += \
    qmqtt/src/mqtt/qmqtt.pro

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
