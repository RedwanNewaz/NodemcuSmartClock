#include "backend.h"
#include <QDebug>

backend::backend(QObject *parent) : QObject(parent)
{
    m_settings = std::make_unique<QSettings>("AzanClock", "mqtt");


    m_azanTime = "05:40";
    m_azanName = "Fajr";
    m_ipAddr = "192.168.1.1";
    m_portAddr = "1883";

    m_hourVal = "00";
    m_minuteVal = "00";

    m_hourClock = 20;
    m_minuteClock = 54;
    m_hourAlarm = 13;
    m_minuteAlarm = 12;

    // check whether user changed mqtt setting
    const auto mqtt_user_ip = m_settings->value("ipAddr", QString()).toString();
    const auto mqtt_user_port = m_settings->value("portAddr", QString()).toString();
    if(!mqtt_user_ip.isEmpty())
        m_ipAddr = mqtt_user_ip;
    if(!mqtt_user_port.isEmpty())
        m_portAddr = mqtt_user_port;

    // update clock
//    const auto azanTime = m_settings->value("azanTime", int()).toInt();
//    const auto azanName = m_settings->value("azanName", QString()).toString();
//    const auto hourClock = m_settings->value("hourClock", int()).toInt();
//    const auto minuteClock = m_settings->value("minuteClock", int()).toInt();

    m_azanTime = m_settings->value("azanTime", QString()).toString();
    m_azanName = m_settings->value("azanName", QString()).toString();
    m_hourClock = m_settings->value("hourClock", int()).toInt();
    m_minuteClock = m_settings->value("minuteClock", int()).toInt();

}

QString backend::azanTime()
{
    return m_azanTime;
}

void backend::setAzanTime(const QString &value)
{
    m_azanTime = value;
    emit setAzanTimeChanged();

    // remember this value
    m_settings->setValue("azanTime", value);
}

QString backend::azanName()
{
    return m_azanName;
}

void backend::setAzanName(const QString &value)
{
    m_azanName = value;
    emit setAzanNameChanged();

    // remember this value
    m_settings->setValue("azanName", value);
}

QString backend::ipAddr()
{
    return m_ipAddr;
}

void backend::setIPAddr(const QString &value)
{
     m_ipAddr = value;
     qDebug() << "[IP ADDRESS]: " << value;
     m_settings->setValue("ipAddr", value);
     emit setipAddrChanged();
}

QString backend::portAddr()
{
    return m_portAddr;
}

void backend::setPortAddr(const QString &value)
{
     m_portAddr = value;
     m_settings->setValue("portAddr", value);
     emit setPortAddrChanged();
}

QString backend::hourVal()
{
    return m_hourVal;
}

void backend::setHourVal(const QString &value)
{
     m_portAddr = value;
     emit setHourValChanged();
}

QString backend::minuteVal()
{
    return m_minuteVal;
}

QString backend::sendAlarm()
{
    return m_sendAlarm;
}

void backend::setMinuteVal(const QString &value)
{
     m_minuteVal = value;
     emit setMinuteValChanged();
}

void backend::setSendAlarm(const QString &value)
{
    m_sendAlarm = value;
    emit setSendAlarmChanged();
    emit sendAlarmMqtt(value);
}

int backend::hourClock()
{
    return m_hourClock;
}

void backend::setHourClock(const int &value)
{
    m_hourClock = value;
    emit setHourClockChanged();

    // remember this value
    m_settings->setValue("hourClock", value);
}

int backend::minuteClock()
{
    return m_minuteClock;
}

void backend::setMinuteClock(const int &value)
{
    m_minuteClock = value;
    emit setMinuteClockChanged();

    // remember this value
    m_settings->setValue("minuteClock", value);

}

int backend::hourAlarm()
{
    return m_hourAlarm;
}

void backend::setHourAlarm(const int &value)
{
    m_hourAlarm = value;
    emit setHourAlarmChanged();


}

int backend::minuteAlarm()
{
    return m_minuteAlarm;
}

void backend::setMinuteAlarm(const int &value)
{
    m_minuteAlarm = value;
    emit setMinuteAlarmChanged();
}

