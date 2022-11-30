#ifndef MQTTCLOCK_H
#define MQTTCLOCK_H
#include <qmqtt.h>
#include <QCoreApplication>
#include <QTimer>
#include <QHostInfo>
#include <QObject>
#include <QStringList>
#include "backend.h"

class MqttClock : public QMQTT::Client
{
    Q_OBJECT
public:
    explicit MqttClock(const QStringList& subscribers,
                       const QHostAddress& host,
                       const quint16 port,
                       BackendPtr backendPtr,
                       QObject *parent = nullptr);
    virtual ~MqttClock() {};
signals:

public slots:
    void onConnected();
    void onTimeout();
    void onDisconnected();
    void onSubscribed(const QString& topic);
    void onReceived(const QMQTT::Message& message);
    void sendAlarmMqtt(const QString& topic);

private:
    enum MSG_TYPE{
        CLOCK_TIME = 0,
        ALARM_TIME,
        PRAYER_TIME,
        PRAYER_NAME,
        UNKNOWN
    };

    MSG_TYPE getTopicType(const QString& topic);
signals:
    void setAzanTime(const QString &value);
    void setAzanName(const QString &value);
    void setHourVal(const QString &value);
    void setMinuteVal(const QString &value);

    void setHourClock(const int &value);
    void setMinuteClock(const int &value);
    void setHourAlarm(const int &value);
    void setMinuteAlarm(const int &value);

private:
    BackendPtr m_backendPtr;
    quint16 m_port;
    QHostAddress m_serverAddress;
    QTimer m_timer;
    QStringList m_subsribers;

};

#endif // MQTTCLOCK_H
