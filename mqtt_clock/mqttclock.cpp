#include "mqttclock.h"
#include <QDebug>

MqttClock::MqttClock(const QStringList& subscribers,
                     const QHostAddress& host,
                     const quint16 port,
                     BackendPtr backendPtr,
                     QObject *parent) : QMQTT::Client(host, port, parent), m_subsribers(subscribers), m_backendPtr(backendPtr)
{
    connect(this, &MqttClock::connected, this, &MqttClock::onConnected);
    connect(this, &MqttClock::subscribed, this, &MqttClock::onSubscribed);
    connect(this, &MqttClock::received, this, &MqttClock::onReceived);

    connect(&m_timer, &QTimer::timeout, this, &MqttClock::onTimeout);
    connect(this, &MqttClock::disconnected, this, &MqttClock::onDisconnected);

    // gui solts
    connect(this, &MqttClock::setAzanTime, m_backendPtr.get(), &backend::setAzanTime);
    connect(this, &MqttClock::setAzanName, m_backendPtr.get(), &backend::setAzanName);
    connect(this, &MqttClock::setHourVal, m_backendPtr.get(), &backend::setHourVal);
    connect(this, &MqttClock::setMinuteVal, m_backendPtr.get(), &backend::setMinuteVal);

    connect(this, &MqttClock::setHourClock, m_backendPtr.get(), &backend::setHourClock);
    connect(this, &MqttClock::setMinuteClock, m_backendPtr.get(), &backend::setMinuteClock);
    connect(this, &MqttClock::setHourAlarm, m_backendPtr.get(), &backend::setHourAlarm);
    connect(this, &MqttClock::setMinuteAlarm, m_backendPtr.get(), &backend::setMinuteAlarm);

    connect(m_backendPtr.get(), &backend::sendAlarmMqtt, this, &MqttClock::sendAlarmMqtt);

}

void MqttClock::onConnected()
{
    for(auto& sub: m_subsribers)
    {
         subscribe(sub);
    }
    m_timer.start(100);
}

void MqttClock::onTimeout()
{

}

void MqttClock::onDisconnected()
{
    qDebug() << "disconnected ";
}

void MqttClock::onSubscribed(const QString &topic)
{
    qDebug() << "subscribed " << topic;
}

void MqttClock::onReceived(const QMQTT::Message &message)
{
//    qDebug() << message.topic() << message.payload();

    auto msg = message.payload();
    switch (getTopicType(message.topic()))
    {
        case CLOCK_TIME:
        {
            qDebug()<< "[CLOCK_TIME]: " << msg;
            auto hhmm = msg.split(':');
            emit setHourClock(hhmm[0].toInt());
            emit setMinuteClock(hhmm[1].toInt());
            break;
        }
        case ALARM_TIME:
        {
            if(msg == "0")
            {
                emit setHourAlarm(0);
                emit setMinuteAlarm(0);
                qDebug()<< "[ALARM_TIME]: " << msg;
                break;
            }
            else
            {
                auto hhmm = msg.split(':');
                emit setHourAlarm(hhmm[0].toInt());
                emit setMinuteAlarm(hhmm[1].toInt());
                qDebug()<< "[ALARM_TIME]: " << msg;
                break;
            }
        }
    case PRAYER_TIME: qDebug()<< "[PRAYER_TIME]: " << msg; emit setAzanTime(msg);  break;
    case PRAYER_NAME: qDebug()<< "[PRAYER_NAME]: " << msg; emit setAzanName(msg); break;
    default:
        qDebug() << "unkown type";
    }

}

void MqttClock::sendAlarmMqtt(const QString &topic)
{
    qDebug() << "[will send to Mqtt] " << topic;
    auto hhmm = topic.split(':');
    static const char* hourTopic = "/clock/alarm/set/hour";
    static const char* minuteTopic = "/clock/alarm/set/minute";
    static const char* startTopic = "/clock/alarm/start";
    QMQTT::Message message1(1, hourTopic, QString("%1").arg(hhmm[0]).toUtf8());
    QMQTT::Message message2(2, minuteTopic, QString("%1").arg(hhmm[1]).toUtf8());
    QMQTT::Message message3(3, startTopic, QString("%1").arg("").toUtf8());

    publish(message1);
    publish(message2);
    publish(message3);
}

MqttClock::MSG_TYPE MqttClock::getTopicType(const QString &topic)
{
    for(int i = 0; i < m_subsribers.size(); ++i)
    {
        if(topic == m_subsribers[i])
            return static_cast<MSG_TYPE>(i);
    }
    return UNKNOWN;
}
